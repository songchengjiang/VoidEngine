#include "Material.h"
#include "FrameBufferObject.h"
#include "Light.h"
#include "SceneManager.h"
#include "TextureManager.h"
#include <algorithm>

vePass* vePass::CURRENT_PASS = nullptr;

vePass::vePass()
	: USE_VE_PTR_INIT
, _renderPass(vePass::DEFERRED_PASS)
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
	applyProgram(command);
	applyUniforms(command);
	//if (CURRENT_PASS == this)
	//	return true;
	//CURRENT_PASS = this;
    
	unsigned int texUnit = 0;
	for (auto &tex : _textures) {
		glActiveTexture(GL_TEXTURE0 + texUnit);
		tex.second->bind(command.contextID);
		++texUnit;
	}

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

void vePass::setTexture(TextureType type, veTexture *texture)
{
	if (!texture) return;
	int id = findTextureID(type);
	if (id < 0) {
		_textures.push_back(std::make_pair(type, texture));
	}else {
		_textures[id].second = texture;
	}
}

//veTexture* vePass::getTexture(size_t idx)
//{
//	veAssert(idx < _textures.size());
//	return _textures[idx].get();
//}

const veTexture* vePass::getTexture(size_t idx) const
{
	veAssert(idx < _textures.size());
	return _textures[idx].second.get();
}

veTexture* vePass::getTexture(TextureType type)
{
	int id = findTextureID(type);
	if (id < 0)
		return nullptr;
	return _textures[id].second.get();
}

const veTexture* vePass::getTexture(TextureType type) const
{
	int id = findTextureID(type);
	if (id < 0)
		return nullptr;
	return _textures[id].second.get();
}

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

void vePass::applyProgram(const veRenderCommand &command)
{
    auto program = _programBuffer->getData(command.contextID);
	if (_needLinkProgram || !program) {
		//glUseProgram(0);
        _programBuffer->destroyData(command.contextID);
        program = _programBuffer->createData(command.contextID);
			
		for (auto &iter : _shaders) {
			GLuint id = iter.second->compile();
			glAttachShader(program, id);
		}
		if (_transformFeedback.valid()) {
			auto &varyingList = _transformFeedback->getVaryingList();
			glTransformFeedbackVaryings(program, GLsizei(varyingList.size()), &varyingList[0], GL_INTERLEAVED_ATTRIBS);
		}

		glLinkProgram(program);
		_needLinkProgram = false;
	}
	glUseProgram(program);
}

void vePass::applyUniforms(const veRenderCommand &command)
{
	for (auto &uniform : _uniforms) {
		uniform->apply(command);
	}
}

int vePass::findTextureID(TextureType type) const
{
	for (int i = 0; i < int(_textures.size()); ++i) {
		if (_textures[i].first == type)
			return i;
	}
	return -1;
}

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