#include "Material.h"
#include "FrameBufferObject.h"
#include "Light.h"
#include "SceneManager.h"
#include "Constants.h"
#include "TextureManager.h"
#include "NodeVisitor.h"
#include <algorithm>

class veLightVisitor : public veNodeVisitor
{
public:
    veLightVisitor(){}
    
    virtual bool visit(veNode &node) {
        if (node.isVisible()) {
            veLight *light = node.getComponent<veDirectionalLight>();
            if (light) {
                lightList.push_back(light);
            }
            light = node.getComponent<vePointLight>();
            if (light) {
                lightList.push_back(light);
            }
        }
        return false;
    }
    
    std::list<veLight*> lightList;
};

vePass* vePass::CURRENT_PASS = nullptr;

vePass::vePass()
	: USE_VE_PTR_INIT
	, _depthTest(true)
	, _depthWirte(true)
	, _cullFace(true)
	, _stencilTest(false)
	, _cullFaceMode(GL_BACK)
	, _blendFunc(veBlendFunc::DISABLE)
	, _blendEquation(GL_FUNC_ADD)
	, _stencilFunc(veStencilFunc::ALWAYS)
	, _stencilOp(veStencilOp::KEEP)
	, _castShadow(true)
	, _needLinkProgram(true)
	, _mask(0xffffffff)
{
    _programBuffer = veGLDataBufferManager::instance()->createGLDataBuffer([]() -> GLuint{
        return glCreateProgram();
    }, [](GLuint prog){
        glDeleteProgram(prog);
    });
}

vePass::~vePass()
{
}

void vePass::visit(const veRenderCommand &command)
{
}

bool vePass::apply(const veRenderCommand &command)
{
    
    if (command.camera->getIrradianceTexture()) {
        auto uniform = this->getUniform(SHADER_INNER_UNIFROM_IRRADIANCE_NAME);
        if (!uniform) {
            uniform = new veUniform(SHADER_INNER_UNIFROM_IRRADIANCE_NAME);
            this->addUniform(uniform);
        }
        veTexture *texture = nullptr;
        if (!uniform->getValue(texture) || texture != command.camera->getIrradianceTexture()) {
            uniform->setValue(command.camera->getIrradianceTexture());
            this->getShader(veShader::FRAGMENT_SHADER)->addDefination(SHADER_DEFINATION_IBL_NAME);
            _needLinkProgram = true;
        }
    }


    if (command.camera->getRadianceTexture()) {
        auto uniform = this->getUniform(SHADER_INNER_UNIFROM_RADIANCE_NAME);
        if (!uniform) {
            uniform = new veUniform(SHADER_INNER_UNIFROM_RADIANCE_NAME);
            this->addUniform(uniform);
        }
        veTexture *texture = nullptr;
        if (!uniform->getValue(texture) || texture != command.camera->getRadianceTexture()) {
            uniform->setValue(command.camera->getRadianceTexture());
            this->getShader(veShader::FRAGMENT_SHADER)->addDefination(SHADER_DEFINATION_IBL_NAME);
            _needLinkProgram = true;
        }
    }
    
    if (command.camera->getLutTexture()) {
        auto uniform = this->getUniform(SHADER_INNER_UNIFROM_LUT_NAME);
        if (!uniform) {
            uniform = new veUniform(SHADER_INNER_UNIFROM_LUT_NAME);
            this->addUniform(uniform);
        }
        veTexture *texture = nullptr;
        if (!uniform->getValue(texture) || texture != command.camera->getLutTexture()) {
            uniform->setValue(command.camera->getLutTexture());
            this->getShader(veShader::FRAGMENT_SHADER)->addDefination(SHADER_DEFINATION_IBL_NAME);
            _needLinkProgram = true;
        }
    }
    
    veLightVisitor lightVisitor;
    command.sceneManager->getRootNode()->accept(lightVisitor);
    bool hasOneLight = false;
    if (!lightVisitor.lightList.empty()) {
        const auto &lightList = lightVisitor.lightList;
        for (auto &light : lightList) {
            if (light->getLightType() == veLight::DIRECTIONAL) {
                if (light->isEnabled()) {
                    auto uniform = this->getUniform(SHADER_INNER_UNIFROM_DIR_LIGHT_DIRECTION_NAME);
                    if (!uniform) {
                        uniform = new veUniform(SHADER_INNER_UNIFROM_DIR_LIGHT_DIRECTION_NAME);
                        this->addUniform(uniform);
                        _needLinkProgram = true;
                    }
                    veVec4 lightDir = light->getAttachedNode()->getNodeToWorldMatrix() * veVec4(0.0f, 0.0f, 1.0f, 0.0f);
                    uniform->setValue(veVec3(lightDir.x(), lightDir.y(), lightDir.z()));
                    
                    uniform = this->getUniform(SHADER_INNER_UNIFROM_DIR_LIGHT_COlOR_NAME);
                    if (!uniform) {
                        uniform = new veUniform(SHADER_INNER_UNIFROM_DIR_LIGHT_COlOR_NAME);
                        this->addUniform(uniform);
                        _needLinkProgram = true;
                    }
                    uniform->setValue(light->getColor());
                    
                    uniform = this->getUniform(SHADER_INNER_UNIFROM_DIR_LIGHT_INTENSITY_NAME);
                    if (!uniform) {
                        uniform = new veUniform(SHADER_INNER_UNIFROM_DIR_LIGHT_INTENSITY_NAME);
                        this->addUniform(uniform);
                        _needLinkProgram = true;
                    }
                    uniform->setValue(light->getIntensity());
                    
                    if (_needLinkProgram) {
                        this->getShader(veShader::FRAGMENT_SHADER)->addDefination(SHADER_DEFINATION_DIR_LIGHTING_NAME);
                    }
                    hasOneLight = true;
                }else {
                    if (this->getShader(veShader::FRAGMENT_SHADER)->removeDefination(SHADER_DEFINATION_DIR_LIGHTING_NAME)) {
                        this->removeUniform(SHADER_INNER_UNIFROM_DIR_LIGHT_DIRECTION_NAME);
                        this->removeUniform(SHADER_INNER_UNIFROM_DIR_LIGHT_COlOR_NAME);
                        this->removeUniform(SHADER_INNER_UNIFROM_DIR_LIGHT_INTENSITY_NAME);
                        _needLinkProgram = true;
                    }
                }
            }else if (light->getLightType() == veLight::POINT) {
                if (light->isEnabled()) {
                    auto uniform = this->getUniform(SHADER_INNER_UNIFROM_POINT_LIGHT_POSITION_NAME);
                    if (!uniform) {
                        uniform = new veUniform(SHADER_INNER_UNIFROM_POINT_LIGHT_POSITION_NAME);
                        this->addUniform(uniform);
                        _needLinkProgram = true;
                    }
                    veVec3 lightPosition, lightScale;
                    light->getAttachedNode()->getNodeToWorldMatrix().decomposition(&lightPosition, &lightScale, nullptr);
                    uniform->setValue(lightPosition);
                    
                    uniform = this->getUniform(SHADER_INNER_UNIFROM_POINT_LIGHT_COlOR_NAME);
                    if (!uniform) {
                        uniform = new veUniform(SHADER_INNER_UNIFROM_POINT_LIGHT_COlOR_NAME);
                        this->addUniform(uniform);
                        _needLinkProgram = true;
                    }
                    uniform->setValue(light->getColor());
                    
                    uniform = this->getUniform(SHADER_INNER_UNIFROM_POINT_LIGHT_INTENSITY_NAME);
                    if (!uniform) {
                        uniform = new veUniform(SHADER_INNER_UNIFROM_POINT_LIGHT_INTENSITY_NAME);
                        this->addUniform(uniform);
                        _needLinkProgram = true;
                    }
                    uniform->setValue(light->getIntensity());
                    
                    uniform = this->getUniform(SHADER_INNER_UNIFROM_POINT_LIGHT_ARI_NAME);
                    if (!uniform) {
                        uniform = new veUniform(SHADER_INNER_UNIFROM_POINT_LIGHT_ARI_NAME);
                        this->addUniform(uniform);
                        _needLinkProgram = true;
                    }
                    uniform->setValue(light->getAttenuationRangeInverse() / lightScale.x());
                    
                    if (_needLinkProgram) {
                        this->getShader(veShader::FRAGMENT_SHADER)->addDefination(SHADER_DEFINATION_POINT_LIGHTING_NAME);
                    }
                    hasOneLight = true;
                }else {
                    if (this->getShader(veShader::FRAGMENT_SHADER)->removeDefination(SHADER_DEFINATION_POINT_LIGHTING_NAME)) {
                        this->removeUniform(SHADER_INNER_UNIFROM_POINT_LIGHT_POSITION_NAME);
                        this->removeUniform(SHADER_INNER_UNIFROM_POINT_LIGHT_COlOR_NAME);
                        this->removeUniform(SHADER_INNER_UNIFROM_POINT_LIGHT_INTENSITY_NAME);
                        this->removeUniform(SHADER_INNER_UNIFROM_POINT_LIGHT_ARI_NAME);
                        _needLinkProgram = true;
                    }
                }
            }
        }
    }
    
    if (hasOneLight) {
        if (this->getShader(veShader::FRAGMENT_SHADER)->addDefination(SHADER_DEFINATION_LIGHTING_NAME)) {
            _needLinkProgram = true;
        }
    }else {
        if (this->getShader(veShader::FRAGMENT_SHADER)->removeDefination(SHADER_DEFINATION_LIGHTING_NAME)) {
            _needLinkProgram = true;
        }
    }

	applyProgram(command);
	applyUniforms(command);
	//if (CURRENT_PASS == this)
	//	return true;
	//CURRENT_PASS = this;
    //applyTextures(command);

	command.sceneManager->getRenderState(command.contextID)->setDepthTest(_depthTest);
	command.sceneManager->getRenderState(command.contextID)->setDepthWrite(_depthWirte);
	command.sceneManager->getRenderState(command.contextID)->setCullface(_cullFace);
	command.sceneManager->getRenderState(command.contextID)->setCullfaceMode(_cullFaceMode);
	command.sceneManager->getRenderState(command.contextID)->setBlendFunc(_blendFunc);
	command.sceneManager->getRenderState(command.contextID)->setBlendEquation(_blendEquation);
	command.sceneManager->getRenderState(command.contextID)->setStencilTest(_stencilTest);
	command.sceneManager->getRenderState(command.contextID)->setStencilFunc(_stencilFunc);
	command.sceneManager->getRenderState(command.contextID)->setStencilOp(_stencilOp);
	command.sceneManager->getRenderState(command.contextID)->applyState();

	if (_callback != nullptr)
		_callback();
	return true;
}

void vePass::setShader(veShader *shader)
{
	_shaders[shader->getType()] = shader;
	_needLinkProgram = true;
}

veShader* vePass::getShader(veShader::Type type)
{
	auto iter = _shaders.find(type);
	if (iter == _shaders.end()) return nullptr;

	return _shaders[type].get();
}

//void vePass::addTexture(veTexture *texture)
//{
//	_textures.push_back(texture);
//	_needLinkProgram = true;
//}

//void vePass::setTexture(size_t idx, veTexture *texture)
//{
//	veAssert(idx < _textures.size());
//	if (_textures[idx] == texture) return;
//	_textures[idx] = texture;
//	_needLinkProgram = true;
//}

//void vePass::setTexture(TextureType type, veTexture *texture)
//{
//	if (!texture) return;
//	int id = findTextureID(type);
//	if (id < 0) {
//		_textures.push_back(std::make_pair(type, texture));
//	}else {
//		_textures[id].second = texture;
//	}
//}

//veTexture* vePass::getTexture(size_t idx)
//{
//	veAssert(idx < _textures.size());
//	return _textures[idx].get();
//}

//const veTexture* vePass::getTexture(size_t idx) const
//{
//	veAssert(idx < _textures.size());
//	return _textures[idx].second.get();
//}
//
//veTexture* vePass::getTexture(TextureType type)
//{
//	int id = findTextureID(type);
//	if (id < 0)
//		return nullptr;
//	return _textures[id].second.get();
//}
//
//const veTexture* vePass::getTexture(TextureType type) const
//{
//	int id = findTextureID(type);
//	if (id < 0)
//		return nullptr;
//	return _textures[id].second.get();
//}
//
//int vePass::getTextureUnit(TextureType type) const
//{
//    return findTextureID(type);
//}

//veTexture* vePass::removeTexture(size_t idx)
//{
//	veAssert(idx < _textures.size());
//	veTexture *tex = _textures[idx].get();
//	_textures.erase(_textures.begin() + idx);
//	_needLinkProgram = true;
//	return tex;
//}

void vePass::addUniform(veUniform *uniform)
{
    auto iter = std::find(_uniforms.begin(), _uniforms.end(), uniform);
    if (iter != _uniforms.end())
        return;
    _uniforms.push_back(uniform);
}

veUniform* vePass::getUniform(const std::string &name) const
{
    for (auto &uniform : _uniforms){
        if (uniform->getName() == name)
            return uniform.get();
    }
	return nullptr;
}

veUniform* vePass::getUniform(size_t idx) const
{
    veAssert(idx < _uniforms.size());
    return _uniforms[idx].get();
}

veUniform* vePass::removeUniform(const std::string &name)
{
    veUniform *uniform = nullptr;
    for (auto iter = _uniforms.begin(); iter != _uniforms.end(); ++iter){
        if ((*iter)->getName() == name){
            uniform = (*iter).get();
            _uniforms.erase(iter);
            break;
        }
    }
	return uniform;
}

veUniform* vePass::removeUniform(size_t idx)
{
    veAssert(idx < _uniforms.size());
    veUniform *uniform = _uniforms[idx].get();
    _uniforms.erase(_uniforms.begin() + idx);
    return uniform;
}

void vePass::reloadProgram()
{
	_needLinkProgram = true;
}

GLuint vePass::getProgram(unsigned int contextID)
{
    return _programBuffer->getData(contextID);
}

void vePass::applyProgram(const veRenderCommand &command)
{
    auto program = _programBuffer->getData(command.contextID);
	if (_needLinkProgram || !program) {
		//glUseProgram(0);
        //_programBuffer->destroyData(command.contextID);
        program = _programBuffer->createData(command.contextID);
			
		for (auto &iter : _shaders) {
            if (0 < iter.second->_shader)
                glDetachShader(program, iter.second->_shader);
			GLuint id = iter.second->compile();
			glAttachShader(program, id);
		}

		glLinkProgram(program);
        for (auto &uniform : _uniforms) {
            uniform->refresh();
        }
		_needLinkProgram = false;
	}
	glUseProgram(program);
    int ec = glGetError();
    if (ec != GL_NO_ERROR) {
        veLog("applyProgram: GL ERROR CODE: 0x%x\n", ec);
    }
}

void vePass::applyUniforms(const veRenderCommand &command)
{
    unsigned int texUnit = 0;
	for (auto &uniform : _uniforms) {
		uniform->apply(command, texUnit);
	}
    int ec = glGetError();
    if (ec != GL_NO_ERROR) {
        veLog("applyUniforms: GL ERROR CODE: 0x%x\n", ec);
    }
}

//void vePass::applyTextures(const veRenderCommand &command)
//{
//    unsigned int texUnit = 0;
//    for (auto &tex : _textures) {
//        glActiveTexture(GL_TEXTURE0 + texUnit);
//        tex.second->bind(command.contextID);
//        ++texUnit;
//    }
//    int ec = glGetError();
//    if (ec != GL_NO_ERROR) {
//        veLog("applyTextures: GL ERROR CODE: 0x%x\n", ec);
//    }
//}

//int vePass::findTextureID(TextureType type) const
//{
//	for (int i = 0; i < int(_textures.size()); ++i) {
//		if (_textures[i].first == type)
//			return i;
//	}
//	return -1;
//}

veTechnique::veTechnique()
	: USE_VE_PTR_INIT
{

}

veTechnique::~veTechnique()
{

}

void veTechnique::addPass(vePass *pass)
{
	_passes.push_back(pass);
}

const vePass* veTechnique::getPass(size_t idx) const
{
	veAssert(idx < _passes.size());
	return _passes[idx].get();
}

vePass* veTechnique::getPass(size_t idx)
{
	veAssert(idx < _passes.size());
	return _passes[idx].get();
}

vePass* veTechnique::removePass(size_t idx)
{
	veAssert(idx < _passes.size());
	vePass *pass = _passes[idx].get();
	_passes.erase(_passes.begin() + idx);
	return pass;
}

veMaterial::veMaterial()
	: USE_VE_PTR_INIT
	, _activeTechnique(nullptr)
{

}

veMaterial::~veMaterial()
{

}

void veMaterial::addTechnique(veTechnique *tech)
{
	_techniques.push_back(tech);
	if (!_activeTechnique) {
		_activeTechnique = tech;
	}
}

const veTechnique* veMaterial::getTechnique(size_t idx) const
{
	veAssert(idx < _techniques.size());
	return _techniques[idx].get();
}

veTechnique* veMaterial::getTechnique(size_t idx)
{
	veAssert(idx < _techniques.size());
	return _techniques[idx].get();
}

const veTechnique* veMaterial::getTechnique(const std::string &name) const
{
	for (auto &iter : _techniques) {
		if (iter->getName() == name) {
			return iter.get();
		}
	}
	return nullptr;
}

veTechnique* veMaterial::getTechnique(const std::string &name)
{
	for (auto &iter : _techniques) {
		if (iter->getName() == name) {
			return iter.get();
		}
	}
	return nullptr;
}

veTechnique* veMaterial::removeTechnique(size_t idx)
{
	veAssert(idx < _techniques.size());
	veTechnique *tech = _techniques[idx].get();
	_techniques.erase(_techniques.begin() + idx);
	if (_techniques.empty())
		_activeTechnique = nullptr;
	return tech;
}

veMaterialArray::veMaterialArray()
	: USE_VE_PTR_INIT
{

}

veMaterialArray::~veMaterialArray()
{

}

void veMaterialArray::addMaterial(veMaterial *material)
{
	_materials.push_back(material);
}

const veMaterial* veMaterialArray::getMaterial(size_t idx) const
{
	veAssert(idx < _materials.size());
	return _materials[idx].get();
}

veMaterial* veMaterialArray::getMaterial(size_t idx)
{
	veAssert(idx < _materials.size());
	return _materials[idx].get();
}

const veMaterial* veMaterialArray::getMaterial(const std::string &name) const
{
	for (auto &iter : _materials) {
		if (iter->getName() == name) {
			return iter.get();
		}
	}
	return nullptr;
}

veMaterial* veMaterialArray::getMaterial(const std::string &name)
{
	for (auto &iter : _materials) {
		if (iter->getName() == name) {
			return iter.get();
		}
	}
	return nullptr;
}
