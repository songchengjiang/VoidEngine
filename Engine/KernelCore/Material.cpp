#include "Material.h"
#include "FrameBufferObject.h"
#include "ShaderDefinationGenerator.h"
#include "Light.h"
#include "SceneManager.h"
#include "TextureManager.h"

const std::string veMaterial::SYSTEM_MATERIAL_DIRECTIONAL_SHADOW_MAP_FOR_ANIM_ENTITY = "SYSTEM_MATERIAL_DIRECTIONAL_SHADOW_MAP_FOR_ANIM_ENTITY";
const std::string veMaterial::SYSTEM_MATERIAL_DIRECTIONAL_SHADOW_MAP_FOR_ENTITY = "SYSTEM_MATERIAL_DIRECTIONAL_SHADOW_MAP_FOR_ENTITY";
const std::string veMaterial::SYSTEM_MATERIAL_OMNIDIRECTIONAL_SHADOW_MAP_FOR_ANIM_ENTITY = "SYSTEM_MATERIAL_OMNIDIRECTIONAL_SHADOW_MAP_FOR_ANIM_ENTITY";
const std::string veMaterial::SYSTEM_MATERIAL_OMNIDIRECTIONAL_SHADOW_MAP_FOR_ENTITY = "SYSTEM_MATERIAL_OMNIDIRECTIONAL_SHADOW_MAP_FOR_ENTITY";

const std::string veMaterial::SYSTEM_MATERIAL_LIGHTING_PASS_FOR_ANIM_ENTITY = "SYSTEM_MATERIAL_LIGHTING_PASS_FOR_ANIM_ENTITY";
const std::string veMaterial::SYSTEM_MATERIAL_LIGHTING_PASS_FOR_ENTITY = "SYSTEM_MATERIAL_LIGHTING_PASS_FOR_ENTITY";

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
	, _program(0)
	, _needLinkProgram(true)
	, _mask(0xffffffff)
{
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

	if (CURRENT_PASS == this)
		return true;
	CURRENT_PASS = this;

	unsigned int texUnit = 0;
	for (auto &tex : _textures) {
		tex.second->bind(texUnit);
		++texUnit;
	}
	applyLightTextures(texUnit, command);


	veRenderState::instance()->setDepthTest(_depthTest);
	veRenderState::instance()->setDepthWrite(_depthWirte);
	veRenderState::instance()->setCullface(_cullFace);
	veRenderState::instance()->setCullfaceMode(_cullFaceMode);
	veRenderState::instance()->setBlendFunc(_blendFunc);
	veRenderState::instance()->setBlendEquation(_blendEquation);
	veRenderState::instance()->setStencilTest(_stencilTest);
	veRenderState::instance()->setStencilFunc(_stencilFunc);
	veRenderState::instance()->setStencilOp(_stencilOp);
	veRenderState::instance()->applyState();

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

void vePass::addTexture(TextureType type, veTexture *texture)
{
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

//const veTexture* vePass::getTexture(size_t idx) const
//{
//	veAssert(idx < _textures.size());
//	return _textures[idx].get();
//}

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
	_uniforms[uniform->getName()] = uniform;
}

veUniform* vePass::getUniform(const std::string &name)
{
	auto iter = _uniforms.find(name);
	if (iter != _uniforms.end()) {
		return iter->second.get();
	}
	return nullptr;
}

veUniform* vePass::removeUniform(const std::string &name)
{
	auto iter = _uniforms.find(name);
	if (iter == _uniforms.end()) 
		return nullptr;
	veUniform *uniform = iter->second.get();
	_uniforms.erase(iter);
	return uniform;
}

void vePass::needLink()
{
	_needLinkProgram = true;
}

void vePass::applyProgram(const veRenderCommand &command)
{
	if (_needLinkProgram) {
		//glUseProgram(0);
		if (_program) {
			glDeleteProgram(_program);
			_program = 0;
		}
		if (!_program)
			_program = glCreateProgram();
		ShaderDefinatiosGenerator sdg(command);
		for (auto &iter : _shaders) {
			iter.second->setShaderHeader(iter.first, sdg.getDefinations(iter.first));
			GLuint id = iter.second->compile();
			glAttachShader(_program, id);
		}
		if (_transformFeedback.valid()) {
			auto &varyingList = _transformFeedback->getVaryingList();
			glTransformFeedbackVaryings(_program, GLsizei(varyingList.size()), &varyingList[0], GL_INTERLEAVED_ATTRIBS);
		}

		glLinkProgram(_program);
		locateLightUnifroms(command);
		_needLinkProgram = false;
	}
	glUseProgram(_program);
}

void vePass::applyUniforms(const veRenderCommand &command)
{
	for (auto &iter : _uniforms) {
		iter.second->apply(command);
	}
}

void vePass::applyLightTextures(unsigned int beginTexUnit, const veRenderCommand &command)
{
	const veLightListMap &lightList = command.sceneManager->getLightListMap();
	if (lightList.empty())
		return;

	unsigned int texUnit = beginTexUnit;

	if (0 <= _lightUniformLocations.lightTexture) {
		glUniform1i(_lightUniformLocations.lightTexture, texUnit);
		command.camera->getDeferredLightingTexture()->bind(texUnit);
		++texUnit;
	}

	//if (0 <= _lightUniformLocations.dirLightShadowMap) {
	//	glUniform1i(_lightUniformLocations.dirLightShadowMap, texUnit);
	//	if (veDirectionalLight::getShadowTexture()) {
	//		veDirectionalLight::getShadowTexture()->bind(texUnit);
	//	}
	//	++texUnit;
	//}

	//if (0 <= _lightUniformLocations.pointLightShadowMap) {
	//	glUniform1i(_lightUniformLocations.pointLightShadowMap, texUnit);
	//	if (vePointLight::getShadowTexture())
	//		vePointLight::getShadowTexture()->bind(texUnit);
	//	++texUnit;
	//}

	//if (0 <= _lightUniformLocations.spotLightShadowMap) {
	//	glUniform1i(_lightUniformLocations.spotLightShadowMap, texUnit);
	//	if (veSpotLight::getShadowTexture()) {
	//		veSpotLight::getShadowTexture()->bind(texUnit);
	//	}
	//}
}

void vePass::locateLightUnifroms(const veRenderCommand &command)
{
	if (command.sceneManager->getLightListMap().empty())
		return;

#define UNIFORM_LOC(NAME) glGetUniformLocation(_program, NAME.c_str())

	_lightUniformLocations.lightTexture = UNIFORM_LOC(veLight::DEFUALT_LIGHT_TEXTURE_UNIFORM_NAME);
//
//	_lightUniformLocations.dirLightVisible = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_VISIBLE_NAME);
//	_lightUniformLocations.pointLightVisible = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_VISIBLE_NAME);
//	_lightUniformLocations.spotLightVisible = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_VISIBLE_NAME);
//
//	_lightUniformLocations.dirLightShadowMap = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_NAME);
//	_lightUniformLocations.pointLightShadowMap = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_NAME);
//	_lightUniformLocations.spotLightShadowMap = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_NAME);
//
//	_lightUniformLocations.dirlightParams.assign(9, -1);
//	_lightUniformLocations.pointlightParams.assign(10, -1);
//	_lightUniformLocations.spotlightParams.assign(13, -1);
//
//	_lightUniformLocations.dirlightParams[0] = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_DIRECTION_NAME);
//	_lightUniformLocations.dirlightParams[1] = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME);
//	_lightUniformLocations.dirlightParams[2] = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME);
//	_lightUniformLocations.dirlightParams[3] = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME);
//	_lightUniformLocations.dirlightParams[4] = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME);
//	_lightUniformLocations.dirlightParams[5] = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME);
//	_lightUniformLocations.dirlightParams[6] = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTSHADOW_NAME);
//	_lightUniformLocations.dirlightParams[7] = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTNESS_NAME);
//	_lightUniformLocations.dirlightParams[8] = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MATRIX_NAME);
//
//	_lightUniformLocations.pointlightParams[0] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_POSITION_NAME);
//	_lightUniformLocations.pointlightParams[1] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME);
//	_lightUniformLocations.pointlightParams[2] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME);
//	_lightUniformLocations.pointlightParams[3] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME);
//	_lightUniformLocations.pointlightParams[4] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME);
//	_lightUniformLocations.pointlightParams[5] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME);
//	_lightUniformLocations.pointlightParams[6] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME);
//	_lightUniformLocations.pointlightParams[7] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTSHADOW_NAME);
//	_lightUniformLocations.pointlightParams[8] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTNESS_NAME);
//	_lightUniformLocations.pointlightParams[9] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MATRIX_NAME);
//
//	_lightUniformLocations.spotlightParams[0] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_DIRECTION_NAME);
//	_lightUniformLocations.spotlightParams[1] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_POSITION_NAME);
//	_lightUniformLocations.spotlightParams[2] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME);
//	_lightUniformLocations.spotlightParams[3] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME);
//	_lightUniformLocations.spotlightParams[4] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME);
//	_lightUniformLocations.spotlightParams[5] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_INNER_ANGLE_COS_NAME);
//	_lightUniformLocations.spotlightParams[6] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_OUTER_ANGLE_COS_NAME);
//	_lightUniformLocations.spotlightParams[7] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME);
//	_lightUniformLocations.spotlightParams[8] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME);
//	_lightUniformLocations.spotlightParams[9] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME);
//	_lightUniformLocations.spotlightParams[10] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTSHADOW_NAME);
//	_lightUniformLocations.spotlightParams[11] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTNESS_NAME);
//	_lightUniformLocations.spotlightParams[12] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MATRIX_NAME);
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

const veMaterial* veMaterialArray::getMaterial(unsigned int idx) const
{
	veAssert(idx < _materials.size());
	return _materials[idx].get();
}

veMaterial* veMaterialArray::getMaterial(unsigned int idx)
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