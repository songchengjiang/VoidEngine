#include "Material.h"
#include "FrameBufferObject.h"
#include "ShaderDefinationGenerator.h"
#include "Light.h"
#include "SceneManager.h"

veBlendFunc veBlendFunc::DISABLE = { GL_ONE, GL_ZERO };
veBlendFunc veBlendFunc::ADDITIVE = { GL_SRC_ALPHA, GL_ONE };
veBlendFunc veBlendFunc::ALPHA = { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };

const std::string veMaterial::SYSTEM_MATERIAL_DIRECTIONAL_SHADOW_MAP_FOR_ANIM_ENTITY = "SYSTEM_MATERIAL_DIRECTIONAL_SHADOW_MAP_FOR_ANIM_ENTITY";
const std::string veMaterial::SYSTEM_MATERIAL_DIRECTIONAL_SHADOW_MAP_FOR_ENTITY = "SYSTEM_MATERIAL_DIRECTIONAL_SHADOW_MAP_FOR_ENTITY";
const std::string veMaterial::SYSTEM_MATERIAL_OMNIDIRECTIONAL_SHADOW_MAP_FOR_ANIM_ENTITY = "SYSTEM_MATERIAL_OMNIDIRECTIONAL_SHADOW_MAP_FOR_ANIM_ENTITY";
const std::string veMaterial::SYSTEM_MATERIAL_OMNIDIRECTIONAL_SHADOW_MAP_FOR_ENTITY = "SYSTEM_MATERIAL_OMNIDIRECTIONAL_SHADOW_MAP_FOR_ENTITY";

vePass* vePass::CURRENT_PASS = nullptr;
bool vePass::CURRENT_DEPTH_TEST = false;
bool vePass::CURRENT_DEPTH_WRITE = true;
bool vePass::CURRENT_CULL_FACE = false;
GLenum vePass::CURRENT_CULL_FACE_MODE = GL_BACK;

veBlendFunc vePass::CURRENT_BLEND_FUNC = veBlendFunc::DISABLE;
//GLenum vePass::CURRENT_POLYGON_MODE = GL_FILL;

vePass::vePass()
	: USE_VE_PTR_INIT
	, _depthTest(true)
	, _depthWirte(true)
	, _cullFace(true)
	, _cullFaceMode(GL_BACK)
	, _blendFunc(veBlendFunc::DISABLE)
	//, _polygonMode(GL_FILL)
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

	for (unsigned int i = 0; i < _textures.size(); ++i) {
		auto &tex = _textures[i];
		tex->bind(i);
	}

	if (_depthTest != CURRENT_DEPTH_TEST) {
		_depthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
		CURRENT_DEPTH_TEST = _depthTest;
	}

	if (_depthWirte != CURRENT_DEPTH_WRITE) {
		_depthWirte ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
		CURRENT_DEPTH_WRITE = _depthWirte;
	}
	
	if (_cullFace != CURRENT_CULL_FACE) {
		_cullFace ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
		CURRENT_CULL_FACE = _cullFace;
	}

	if (CURRENT_CULL_FACE) {
		if (_cullFaceMode != CURRENT_CULL_FACE_MODE) {
			glCullFace(_cullFaceMode);
			CURRENT_CULL_FACE_MODE = _cullFaceMode;
		}
	}
	
	//if (_polygonMode != CURRENT_POLYGON_MODE) {
	//	glPolygonMode(GL_FRONT_AND_BACK, _polygonMode);
	//	CURRENT_POLYGON_MODE = _polygonMode;
	//}

	if (_blendFunc != CURRENT_BLEND_FUNC) {
		if (_blendFunc != veBlendFunc::DISABLE) {
			glEnable(GL_BLEND);
			glBlendFunc(_blendFunc.src, _blendFunc.dst);
		}
		else {
			glDisable(GL_BLEND);
		}
		CURRENT_BLEND_FUNC = _blendFunc;
	}

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

void vePass::addTexture(veTexture *texture)
{
	_textures.push_back(texture);
	_needLinkProgram = true;
}

void vePass::setTexture(size_t idx, veTexture *texture)
{
	veAssert(idx < _textures.size());
	if (_textures[idx] == texture) return;
	_textures[idx] = texture;
	_needLinkProgram = true;
}

veTexture* vePass::getTexture(size_t idx)
{
	veAssert(idx < _textures.size());
	return _textures[idx].get();
}

const veTexture* vePass::getTexture(size_t idx) const
{
	veAssert(idx < _textures.size());
	return _textures[idx].get();
}

veTexture* vePass::removeTexture(size_t idx)
{
	veAssert(idx < _textures.size());
	veTexture *tex = _textures[idx].get();
	_textures.erase(_textures.begin() + idx);
	_needLinkProgram = true;
	return tex;
}

void vePass::addUniform(veUniform *uniform)
{
	_uniforms.push_back(uniform);
}

veUniform* vePass::getUniform(size_t idx)
{
	veAssert(idx < _uniforms.size());
	return _uniforms[idx].get();
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

void vePass::restoreGLState()
{
	if (!CURRENT_DEPTH_WRITE) {
		glDepthMask(GL_TRUE);
		CURRENT_DEPTH_WRITE = true;
	}
	CURRENT_PASS = nullptr;
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
	if (lightList.empty() || (_lightUniformLocations.dirlightNum < 0 && _lightUniformLocations.pointlightNum < 0 && _lightUniformLocations.spotlightNum < 0))
		return;

	unsigned int dirLightNum = 0;
	unsigned int pointLightNum = 0;
	unsigned int spotLightNum = 0;
	unsigned int totalNum = 0;
	for (auto &iter : lightList) {
		if (iter->isInScene() && iter->isVisible() && (iter->getMask() & command.mask)) {
			unsigned int lightIdx = 0;
			if (iter->getLightType() == veLight::DIRECTIONAL) {
				lightIdx = dirLightNum;
				++dirLightNum;
			}
			else if (iter->getLightType() == veLight::POINT) {
				lightIdx = pointLightNum;
				++pointLightNum;
			}
			else if (iter->getLightType() == veLight::SPOT) {
				lightIdx = spotLightNum;
				++spotLightNum;
			}
			applyLightUniforms(totalNum, lightIdx, iter.get(), command);
			++totalNum;
		}
	}

	if (0 < _lightUniformLocations.dirlightNum) {
		glUniform1i(_lightUniformLocations.dirlightNum, GLint(dirLightNum));
	}

	if (0 < _lightUniformLocations.pointlightNum) {
		glUniform1i(_lightUniformLocations.pointlightNum, GLint(pointLightNum));
	}

	if (0 < _lightUniformLocations.spotlightNum) {
		glUniform1i(_lightUniformLocations.spotlightNum, GLint(spotLightNum));
	}
}

void vePass::applyUniforms(const veRenderCommand &command)
{
	for (auto &iter : _uniforms) {
		iter->apply(command);
	}
}

void vePass::applyLightUniforms(unsigned int total, unsigned int idx, veLight *light, const veRenderCommand &command)
{
	if (light->getMask() & command.camera->getMask()) {
		light->setLightInCameraMatrix(command.camera->viewMatrix() * light->getNodeToWorldMatrix());
	}

	GLint positionLoc           = -1;
	GLint directionLoc          = -1;
	GLint colorLoc              = -1;
	GLint intensityLoc          = -1;
	GLint attenuationInverseLoc = -1;
	GLint shadowEnabledLoc      = -1;
	GLint shadowBiasLoc         = -1;
	GLint shadowStrengthLoc     = -1;
	GLint lightMatrixLoc        = -1;
	GLint shadowMapLoc          = -1;
	GLint innerAngleCosLoc      = -1;
	GLint outerAngleCosLoc      = -1;
	if (light->getLightType() == veLight::DIRECTIONAL) {
		auto &lightParamLocs  = _lightUniformLocations.dirlightParams;
		directionLoc          = lightParamLocs[0] + idx;
		colorLoc              = lightParamLocs[1] + idx;
		intensityLoc          = lightParamLocs[2] + idx;
		attenuationInverseLoc = lightParamLocs[3] + idx;
		shadowEnabledLoc      = lightParamLocs[4] + idx;
		shadowBiasLoc         = lightParamLocs[5] + idx;
		shadowStrengthLoc     = lightParamLocs[6] + idx;
		lightMatrixLoc        = lightParamLocs[7] + idx;
		shadowMapLoc          = lightParamLocs[8] + idx;
	}
	else if (light->getLightType() == veLight::POINT) {
		auto &lightParamLocs  = _lightUniformLocations.pointlightParams;
		positionLoc           = lightParamLocs[0] + idx;
		colorLoc              = lightParamLocs[1] + idx;
		intensityLoc          = lightParamLocs[2] + idx;
		attenuationInverseLoc = lightParamLocs[3] + idx;
		shadowEnabledLoc      = lightParamLocs[4] + idx;
		shadowBiasLoc         = lightParamLocs[5] + idx;
		shadowStrengthLoc     = lightParamLocs[6] + idx;
		lightMatrixLoc        = lightParamLocs[7] + idx;
		shadowMapLoc          = lightParamLocs[8] + idx;
	}
	else if (light->getLightType() == veLight::SPOT) {
		auto &lightParamLocs  = _lightUniformLocations.spotlightParams;
		directionLoc          = lightParamLocs[0] + idx;
		positionLoc           = lightParamLocs[1] + idx;
		colorLoc              = lightParamLocs[2] + idx;
		intensityLoc          = lightParamLocs[3] + idx;
		attenuationInverseLoc = lightParamLocs[4] + idx;
		innerAngleCosLoc      = lightParamLocs[5] + idx;
		outerAngleCosLoc      = lightParamLocs[6] + idx;
		shadowEnabledLoc      = lightParamLocs[7] + idx;
		shadowBiasLoc         = lightParamLocs[8] + idx;
		shadowStrengthLoc     = lightParamLocs[9] + idx;
		lightMatrixLoc        = lightParamLocs[10] + idx;
		shadowMapLoc          = lightParamLocs[11] + idx;
	}

	veMat4 lightInView = light->getLightInCameraMatrix();

	if (0 < positionLoc) {
		glUniform3f(positionLoc, lightInView[0][3], lightInView[1][3], lightInView[2][3]);
	}

	if (0 < directionLoc) {
		lightInView[0][3] = lightInView[1][3] = lightInView[2][3] = 0.0f;
		veVec3 direction = lightInView * -veVec3::UNIT_Z;
		direction.normalize();
		glUniform3f(directionLoc, direction.x(), direction.y(), direction.z());
	}
	if (0 < colorLoc) {
		const auto &color = light->getColor();
		glUniform3f(colorLoc, color.r(), color.g(), color.b());
	}
	if (0 < intensityLoc) {
		glUniform1f(intensityLoc, light->getIntensity());
	}

	if (0 < innerAngleCosLoc) {
		glUniform1f(innerAngleCosLoc, static_cast<veSpotLight *>(light)->getInnerAngleCos());
	}
	if (0 < outerAngleCosLoc) {
		glUniform1f(outerAngleCosLoc, static_cast<veSpotLight *>(light)->getOuterAngleCos());
	}

	if (0 < shadowEnabledLoc) {
		glUniform1f(shadowEnabledLoc, light->isShadowEnabled() ? 1.0f : 0.0f);
	}

	if (light->isShadowEnabled()) {
		if (0 < shadowBiasLoc) {
			glUniform1f(shadowBiasLoc, light->getShadowBias());
		}
		if (0 < shadowStrengthLoc) {
			glUniform1f(shadowStrengthLoc, light->getShadowStrength());
		}
		if (0 < lightMatrixLoc) {
			veMat4 lightMat = light->getLightMatrix() * command.worldMatrix->value();
			float m[16];
			m[0] = lightMat[0][0]; m[4] = lightMat[0][1]; m[8]  = lightMat[0][2]; m[12] = lightMat[0][3];
			m[1] = lightMat[1][0]; m[5] = lightMat[1][1]; m[9]  = lightMat[1][2]; m[13] = lightMat[1][3];
			m[2] = lightMat[2][0]; m[6] = lightMat[2][1]; m[10] = lightMat[2][2]; m[14] = lightMat[2][3];
			m[3] = lightMat[3][0]; m[7] = lightMat[3][1]; m[11] = lightMat[3][2]; m[15] = lightMat[3][3];
			glUniformMatrix4fv(lightMatrixLoc, 1, GL_FALSE, m);
		}
		if (0 < shadowMapLoc) {
			glUniform1i(shadowMapLoc, total + _textures.size());
			light->getShadowTexture()->bind(total + _textures.size());
		}
	}
}

void vePass::locateLightUnifroms(const veRenderCommand &command)
{
	if (!veDirectionalLight::TOTAL_LIGHT_NUM && !vePointLight::TOTAL_LIGHT_NUM && veSpotLight::TOTAL_LIGHT_NUM)
		return;

#define UNIFORM_LOC(NAME) glGetUniformLocation(_program, NAME.c_str())

	_lightUniformLocations.dirlightNum   = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME);
	_lightUniformLocations.pointlightNum = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME);
	_lightUniformLocations.spotlightNum  = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME);

	_lightUniformLocations.dirlightParams.assign(9, -1);
	_lightUniformLocations.pointlightParams.assign(9, -1);
	_lightUniformLocations.spotlightParams.assign(12, -1);

	_lightUniformLocations.dirlightParams[0] = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_DIRECTION_NAME);
	_lightUniformLocations.dirlightParams[1] = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME);
	_lightUniformLocations.dirlightParams[2] = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME);
	_lightUniformLocations.dirlightParams[4] = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME);
	_lightUniformLocations.dirlightParams[5] = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME);
	_lightUniformLocations.dirlightParams[6] = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME);
	_lightUniformLocations.dirlightParams[7] = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MATRIX_NAME);
	_lightUniformLocations.dirlightParams[8] = UNIFORM_LOC(veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_NAME);

	_lightUniformLocations.pointlightParams[0] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_POSITION_NAME);
	_lightUniformLocations.pointlightParams[1] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME);
	_lightUniformLocations.pointlightParams[2] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME);
	_lightUniformLocations.pointlightParams[3] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME);
	_lightUniformLocations.pointlightParams[4] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME);
	_lightUniformLocations.pointlightParams[5] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME);
	_lightUniformLocations.pointlightParams[6] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME);
	_lightUniformLocations.pointlightParams[7] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MATRIX_NAME);
	_lightUniformLocations.pointlightParams[8] = UNIFORM_LOC(vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_NAME);

	_lightUniformLocations.spotlightParams[0] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_DIRECTION_NAME);
	_lightUniformLocations.spotlightParams[1] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_POSITION_NAME);
	_lightUniformLocations.spotlightParams[2] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME);
	_lightUniformLocations.spotlightParams[3] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME);
	_lightUniformLocations.spotlightParams[4] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME);
	_lightUniformLocations.spotlightParams[5] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_INNER_ANGLE_COS_NAME);
	_lightUniformLocations.spotlightParams[6] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_OUTER_ANGLE_COS_NAME);
	_lightUniformLocations.spotlightParams[7] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME);
	_lightUniformLocations.spotlightParams[8] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME);
	_lightUniformLocations.spotlightParams[9] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME);
	_lightUniformLocations.spotlightParams[10] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MATRIX_NAME);
	_lightUniformLocations.spotlightParams[11] = UNIFORM_LOC(veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_NAME);
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
	for (auto &iter : _techniques){
		if (iter->getName() == name){
			return iter.get();
		}
	}
	return nullptr;
}

veTechnique* veMaterial::getTechnique(const std::string &name)
{
	for (auto &iter : _techniques){
		if (iter->getName() == name){
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
	for (auto &iter : _materials){
		if (iter->getName() == name){
			return iter.get();
		}
	}
	return nullptr;
}

veMaterial* veMaterialArray::getMaterial(const std::string &name)
{
	for (auto &iter : _materials){
		if (iter->getName() == name){
			return iter.get();
		}
	}
	return nullptr;
}
