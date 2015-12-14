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
	applyLightsUniforms(command);
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

void vePass::setTexture(TextureType type, veTexture *texture)
{
	auto iter = _textures.find(type);
	if (iter != _textures.end()) {
		_textures.erase(iter);
	}
	if (texture)
		_textures[type] = texture;
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
	auto iter = _textures.find(type);
	if (iter == _textures.end())
		return nullptr;
	return iter->second.get();
}

const veTexture* vePass::getTexture(TextureType type) const
{
	auto iter = _textures.find(type);
	if (iter == _textures.end())
		return nullptr;
	return iter->second.get();
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
	_uniforms.push_back(uniform);
}

veUniform* vePass::getUniform(size_t idx)
{
	veAssert(idx < _uniforms.size());
	return _uniforms[idx].get();
}

veUniform* vePass::getUniform(const std::string &name)
{
	for (auto &u : _uniforms) {
		if (u->getName() == name)
			return u.get();
	}
	return nullptr;
}

veUniform* vePass::removeUniform(size_t idx)
{
	veAssert(idx < _uniforms.size());
	veUniform *uniform = _uniforms[idx].get();
	_uniforms.erase(_uniforms.begin() + idx);
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

void vePass::applyLightsUniforms(const veRenderCommand &command)
{
	const veLightList &lightList = command.sceneManager->getLightList();
	if (lightList.empty() || (_lightUniformLocations.dirLightVisible < 0 && _lightUniformLocations.pointLightVisible < 0 && _lightUniformLocations.spotLightVisible < 0))
		return;

#define APPLY_LIGHT_UNIFORM(TYPE) \
	{\
		unsigned int lightIdx = 0;  \
		for (auto &iter : lightList) { \
			if (iter->getLightType() == TYPE) {      \
				applyLightUniforms(lightIdx, iter.get(), command);    \
				++lightIdx;   \
			}  \
		}   \
	}

	APPLY_LIGHT_UNIFORM(veLight::DIRECTIONAL);
	APPLY_LIGHT_UNIFORM(veLight::POINT);
	APPLY_LIGHT_UNIFORM(veLight::SPOT);
}

void vePass::applyUniforms(const veRenderCommand &command)
{
	for (auto &iter : _uniforms) {
		iter->apply(command);
	}
}

void vePass::applyLightUniforms(unsigned int idx, veLight *light, const veRenderCommand &command)
{
	//bool isLightVisible = light->isInScene() && light->isVisible() && (light->getMask() & command.mask);
	//if (light->getLightType() == veLight::DIRECTIONAL) {
	//	if (0 <= _lightUniformLocations.dirLightVisible) {
	//		glUniform1f(_lightUniformLocations.dirLightVisible + idx, isLightVisible ? 1.0f : 0.0f);
	//	}
	//}
	//else if (light->getLightType() == veLight::POINT) {
	//	if (0 <= _lightUniformLocations.pointLightVisible) {
	//		glUniform1f(_lightUniformLocations.pointLightVisible + idx, isLightVisible ? 1.0f : 0.0f);
	//	}
	//}
	//else if (light->getLightType() == veLight::SPOT) {
	//	if (0 <= _lightUniformLocations.spotLightVisible) {
	//		glUniform1f(_lightUniformLocations.spotLightVisible + idx, isLightVisible ? 1.0f : 0.0f);
	//	}
	//}

	//if (isLightVisible) {
	//	if (light->getMask() & command.camera->getMask()) {
	//		light->setLightInCameraMatrix(command.camera->viewMatrix() * light->getNodeToWorldMatrix());
	//	}

	//	GLint positionLoc = -1;
	//	GLint directionLoc = -1;
	//	GLint colorLoc = -1;
	//	GLint intensityLoc = -1;
	//	GLint attenuationInverseLoc = -1;
	//	GLint shadowEnabledLoc = -1;
	//	GLint shadowBiasLoc = -1;
	//	GLint shadowStrengthLoc = -1;
	//	GLint shadowSoftLoc = -1;
	//	GLint shadowSoftnessLoc = -1;
	//	GLint lightMatrixLoc = -1;
	//	GLint innerAngleCosLoc = -1;
	//	GLint outerAngleCosLoc = -1;
	//	if (light->getLightType() == veLight::DIRECTIONAL) {
	//		auto &lightParamLocs = _lightUniformLocations.dirlightParams;
	//		directionLoc = lightParamLocs[0] + idx;
	//		colorLoc = lightParamLocs[1] + idx;
	//		intensityLoc = lightParamLocs[2] + idx;
	//		shadowEnabledLoc = lightParamLocs[3] + idx;
	//		shadowBiasLoc = lightParamLocs[4] + idx;
	//		shadowStrengthLoc = lightParamLocs[5] + idx;
	//		shadowSoftLoc = lightParamLocs[6] + idx;
	//		shadowSoftnessLoc = lightParamLocs[7] + idx;
	//		lightMatrixLoc = lightParamLocs[8] + idx;
	//	}
	//	else if (light->getLightType() == veLight::POINT) {
	//		auto &lightParamLocs = _lightUniformLocations.pointlightParams;
	//		positionLoc = lightParamLocs[0] + idx;
	//		colorLoc = lightParamLocs[1] + idx;
	//		intensityLoc = lightParamLocs[2] + idx;
	//		attenuationInverseLoc = lightParamLocs[3] + idx;
	//		shadowEnabledLoc = lightParamLocs[4] + idx;
	//		shadowBiasLoc = lightParamLocs[5] + idx;
	//		shadowStrengthLoc = lightParamLocs[6] + idx;
	//		shadowSoftLoc = lightParamLocs[7] + idx;
	//		shadowSoftnessLoc = lightParamLocs[8] + idx;
	//		lightMatrixLoc = lightParamLocs[9] + idx;
	//	}
	//	else if (light->getLightType() == veLight::SPOT) {
	//		auto &lightParamLocs = _lightUniformLocations.spotlightParams;
	//		directionLoc = lightParamLocs[0] + idx;
	//		positionLoc = lightParamLocs[1] + idx;
	//		colorLoc = lightParamLocs[2] + idx;
	//		intensityLoc = lightParamLocs[3] + idx;
	//		attenuationInverseLoc = lightParamLocs[4] + idx;
	//		innerAngleCosLoc = lightParamLocs[5] + idx;
	//		outerAngleCosLoc = lightParamLocs[6] + idx;
	//		shadowEnabledLoc = lightParamLocs[7] + idx;
	//		shadowBiasLoc = lightParamLocs[8] + idx;
	//		shadowStrengthLoc = lightParamLocs[9] + idx;
	//		shadowSoftLoc = lightParamLocs[10] + idx;
	//		shadowSoftnessLoc = lightParamLocs[11] + idx;
	//		lightMatrixLoc = lightParamLocs[12] + idx;
	//	}

	//	veMat4 lightInView = light->getLightInCameraMatrix();

	//	if (0 <= positionLoc) {
	//		glUniform3f(positionLoc, lightInView[0][3], lightInView[1][3], lightInView[2][3]);
	//	}

	//	if (0 <= directionLoc) {
	//		lightInView[0][3] = lightInView[1][3] = lightInView[2][3] = 0.0f;
	//		veVec3 direction = lightInView * -veVec3::UNIT_Z;
	//		direction.normalize();
	//		glUniform3f(directionLoc, direction.x(), direction.y(), direction.z());
	//	}
	//	if (0 <= colorLoc) {
	//		const auto &color = light->getColor();
	//		glUniform3f(colorLoc, color.r(), color.g(), color.b());
	//	}
	//	if (0 <= intensityLoc) {
	//		glUniform1f(intensityLoc, light->getIntensity());
	//	}

	//	if (0 <= innerAngleCosLoc) {
	//		glUniform1f(innerAngleCosLoc, static_cast<veSpotLight *>(light)->getInnerAngleCos());
	//	}
	//	if (0 <= outerAngleCosLoc) {
	//		glUniform1f(outerAngleCosLoc, static_cast<veSpotLight *>(light)->getOuterAngleCos());
	//	}

	//	if (0 <= shadowEnabledLoc) {
	//		glUniform1f(shadowEnabledLoc, light->isShadowEnabled() ? 1.0f : 0.0f);
	//	}

	//	if (light->isShadowEnabled()) {
	//		if (0 <= shadowBiasLoc) {
	//			glUniform1f(shadowBiasLoc, light->getShadowBias());
	//		}
	//		if (0 <= shadowStrengthLoc) {
	//			glUniform1f(shadowStrengthLoc, light->getShadowStrength());
	//		}

	//		if (0 <= shadowSoftLoc) {
	//			glUniform1f(shadowSoftLoc, light->isUseSoftShadow() ? 1.0f : 0.0f);
	//		}

	//		if (0 <= shadowSoftnessLoc) {
	//			glUniform1f(shadowSoftnessLoc, light->getShadowSoftness());
	//		}

	//		if (0 <= lightMatrixLoc) {
	//			veMat4 lightMat = light->getLightMatrix() * command.worldMatrix->value();
	//			float m[16];
	//			m[0] = lightMat[0][0]; m[4] = lightMat[0][1]; m[8] = lightMat[0][2]; m[12] = lightMat[0][3];
	//			m[1] = lightMat[1][0]; m[5] = lightMat[1][1]; m[9] = lightMat[1][2]; m[13] = lightMat[1][3];
	//			m[2] = lightMat[2][0]; m[6] = lightMat[2][1]; m[10] = lightMat[2][2]; m[14] = lightMat[2][3];
	//			m[3] = lightMat[3][0]; m[7] = lightMat[3][1]; m[11] = lightMat[3][2]; m[15] = lightMat[3][3];
	//			glUniformMatrix4fv(lightMatrixLoc, 1, GL_FALSE, m);
	//		}
	//	}
	//}
}

void vePass::applyLightTextures(unsigned int beginTexUnit, const veRenderCommand &command)
{
	const veLightList &lightList = command.sceneManager->getLightList();
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
	if (command.sceneManager->getLightList().empty())
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