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
		auto &lightParamLocs  = _lightUniformLocations.dirlightParams[idx];
		directionLoc          = lightParamLocs[0];
		colorLoc              = lightParamLocs[1];
		intensityLoc          = lightParamLocs[2];
		attenuationInverseLoc = lightParamLocs[3];
		shadowEnabledLoc      = lightParamLocs[4];
		shadowBiasLoc         = lightParamLocs[5];
		shadowStrengthLoc     = lightParamLocs[6];
		lightMatrixLoc        = lightParamLocs[7];
		shadowMapLoc          = lightParamLocs[8];
	}
	else if (light->getLightType() == veLight::POINT) {
		auto &lightParamLocs  = _lightUniformLocations.pointlightParams[idx];
		positionLoc           = lightParamLocs[0];
		colorLoc              = lightParamLocs[1];
		intensityLoc          = lightParamLocs[2];
		attenuationInverseLoc = lightParamLocs[3];
		shadowEnabledLoc      = lightParamLocs[4];
		shadowBiasLoc         = lightParamLocs[5];
		shadowStrengthLoc     = lightParamLocs[6];
		lightMatrixLoc        = lightParamLocs[7];
		shadowMapLoc          = lightParamLocs[8];
	}
	else if (light->getLightType() == veLight::SPOT) {
		auto &lightParamLocs  = _lightUniformLocations.spotlightParams[idx];
		directionLoc          = lightParamLocs[0];
		positionLoc           = lightParamLocs[1];
		colorLoc              = lightParamLocs[2];
		intensityLoc          = lightParamLocs[3];
		attenuationInverseLoc = lightParamLocs[4];
		shadowEnabledLoc      = lightParamLocs[5];
		shadowBiasLoc         = lightParamLocs[6];
		shadowStrengthLoc     = lightParamLocs[7];
		lightMatrixLoc        = lightParamLocs[8];
		shadowMapLoc          = lightParamLocs[9];
		innerAngleCosLoc      = lightParamLocs[10];
		outerAngleCosLoc      = lightParamLocs[11];
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
	if (0 < shadowEnabledLoc) {
		glUniform1f(shadowEnabledLoc, light->isShadowEnabled() ? 1.0f : 0.0f);
	}
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
	if (0 < innerAngleCosLoc) {
		glUniform1f(innerAngleCosLoc, static_cast<veSpotLight *>(light)->getInnerAngleCos());
	}
	if (0 < outerAngleCosLoc) {
		glUniform1f(outerAngleCosLoc, static_cast<veSpotLight *>(light)->getOuterAngleCos());
	}


	//glUniform1i(lightParamLocs[0], GLint(light->getLightType()));

	//if (light->getLightType() == veLight::POINT || light->getLightType() == veLight::SPOT) {
	//	glUniform3f(lightParamLocs[1], lightInView[0][3], lightInView[1][3], lightInView[2][3]);
	//}

	//if (light->getLightType() == veLight::DIRECTIONAL || light->getLightType() == veLight::SPOT) {
	//	lightInView[0][3] = lightInView[1][3] = lightInView[2][3] = 0.0f;
	//	veVec3 dir = lightInView * -veVec3::UNIT_Z;
	//	dir.normalize();
	//	glUniform3f(lightParamLocs[2], dir.x(), dir.y(), dir.z());
	//}

	//const auto &color = light->getColor();
	//glUniform3f(lightParamLocs[3], color.r(), color.g(), color.b());
	//glUniform1f(lightParamLocs[4], light->getIntensity());
	//glUniform1f(lightParamLocs[5], light->getAttenuationRangeInverse());

	//if (light->getLightType() == veLight::SPOT) {
	//	glUniform1f(lightParamLocs[6], light->getInnerAngleCos());
	//	glUniform1f(lightParamLocs[7], light->getOuterAngleCos());
	//}

	//glUniform1f(lightParamLocs[8], light->isShadowEnabled()? 1.0f : 0.0f);

	//if (light->isShadowEnabled()) {
	//	glUniform1f(lightParamLocs[9], light->getShadowBias());
	//	glUniform1f(lightParamLocs[10], light->getShadowStrength());

	//	if (0 <= _lightUniformLocations.lightMatrix) {
	//		veMat4 lightMat = light->getLightMatrix() * command.worldMatrix->value();
	//		float m[16];
	//		m[0] = lightMat[0][0]; m[4] = lightMat[0][1]; m[8] = lightMat[0][2]; m[12] = lightMat[0][3];
	//		m[1] = lightMat[1][0]; m[5] = lightMat[1][1]; m[9] = lightMat[1][2]; m[13] = lightMat[1][3];
	//		m[2] = lightMat[2][0]; m[6] = lightMat[2][1]; m[10] = lightMat[2][2]; m[14] = lightMat[2][3];
	//		m[3] = lightMat[3][0]; m[7] = lightMat[3][1]; m[11] = lightMat[3][2]; m[15] = lightMat[3][3];
	//		glUniformMatrix4fv(_lightUniformLocations.lightMatrix + idx, 1, GL_FALSE, m);
	//	}

	//	if (0 <= _lightUniformLocations.lightShadow2D || 0 <= _lightUniformLocations.lightShadowCube) {
	//		if (light->getLightType() == veLight::POINT) {
	//			glUniform1i(_lightUniformLocations.lightShadowCube + idx, idx + _textures.size());
	//		}
	//		else {
	//			glUniform1i(_lightUniformLocations.lightShadow2D + idx, idx + _textures.size());
	//		}			
	//		light->getShadowTexture()->bind(idx + _textures.size());
	//	}
	//}
}

void vePass::locateLightUnifroms(const veRenderCommand &command)
{
	if (command.sceneManager->getLightList().empty())
		return;


	_lightUniformLocations.dirlightNum = glGetUniformLocation(_program, (veDirectionalLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME).c_str());
	_lightUniformLocations.pointlightNum = glGetUniformLocation(_program, (vePointLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME).c_str());
	_lightUniformLocations.spotlightNum = glGetUniformLocation(_program, (veSpotLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME).c_str());

	unsigned int dirLightIdx = 0;
	unsigned int pointLightIdx = 0;
	unsigned int spotLightIdx = 0;
	char lightName[32];
	_lightUniformLocations.dirlightParams.clear();
	_lightUniformLocations.pointlightParams.clear();
	_lightUniformLocations.spotlightParams.clear();
	for (auto &light : command.sceneManager->getLightList()) {
		if (light->isInScene()) {
			if (light->getLightType() == veLight::DIRECTIONAL) {
				sprintf(lightName, "%s[%d].", veDirectionalLight::DEFUALT_LIGHT_UNIFORM_NAME.c_str(), dirLightIdx);
				_lightUniformLocations.dirlightParams.resize(dirLightIdx + 1);
				_lightUniformLocations.dirlightParams[dirLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_DIRECTION_NAME).c_str()));
				_lightUniformLocations.dirlightParams[dirLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME).c_str()));
				_lightUniformLocations.dirlightParams[dirLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME).c_str()));
				_lightUniformLocations.dirlightParams[dirLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME).c_str()));
				_lightUniformLocations.dirlightParams[dirLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME).c_str()));
				_lightUniformLocations.dirlightParams[dirLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME).c_str()));
				_lightUniformLocations.dirlightParams[dirLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME).c_str()));
				_lightUniformLocations.dirlightParams[dirLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_LIGHT_MATRIX_NAME).c_str()));
				_lightUniformLocations.dirlightParams[dirLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_2D_NAME).c_str()));
				++dirLightIdx;
			}
			else if (light->getLightType() == veLight::POINT) {
				sprintf(lightName, "%s[%d].", vePointLight::DEFUALT_LIGHT_UNIFORM_NAME.c_str(), pointLightIdx);
				_lightUniformLocations.pointlightParams.resize(pointLightIdx + 1);
				_lightUniformLocations.pointlightParams[pointLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_POSITION_NAME).c_str()));
				_lightUniformLocations.pointlightParams[pointLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME).c_str()));
				_lightUniformLocations.pointlightParams[pointLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME).c_str()));
				_lightUniformLocations.pointlightParams[pointLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME).c_str()));
				_lightUniformLocations.pointlightParams[pointLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME).c_str()));
				_lightUniformLocations.pointlightParams[pointLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME).c_str()));
				_lightUniformLocations.pointlightParams[pointLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME).c_str()));
				_lightUniformLocations.pointlightParams[pointLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_LIGHT_MATRIX_NAME).c_str()));
				_lightUniformLocations.pointlightParams[pointLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_CUBE_NAME).c_str()));
				++pointLightIdx;
			}
			else if (light->getLightType() == veLight::SPOT) {
				sprintf(lightName, "%s[%d].", veSpotLight::DEFUALT_LIGHT_UNIFORM_NAME.c_str(), spotLightIdx);
				_lightUniformLocations.spotlightParams.resize(spotLightIdx + 1);
				_lightUniformLocations.spotlightParams[spotLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_DIRECTION_NAME).c_str()));
				_lightUniformLocations.spotlightParams[spotLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_POSITION_NAME).c_str()));
				_lightUniformLocations.spotlightParams[spotLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME).c_str()));
				_lightUniformLocations.spotlightParams[spotLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME).c_str()));
				_lightUniformLocations.spotlightParams[spotLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME).c_str()));
				_lightUniformLocations.spotlightParams[spotLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME).c_str()));
				_lightUniformLocations.spotlightParams[spotLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME).c_str()));
				_lightUniformLocations.spotlightParams[spotLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME).c_str()));
				_lightUniformLocations.spotlightParams[spotLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veLight::DEFUALT_LIGHT_UNIFORM_LIGHT_MATRIX_NAME).c_str()));
				_lightUniformLocations.spotlightParams[spotLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_2D_NAME).c_str()));
				_lightUniformLocations.spotlightParams[spotLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veSpotLight::DEFUALT_LIGHT_UNIFORM_INNER_ANGLE_COS_NAME).c_str()));
				_lightUniformLocations.spotlightParams[spotLightIdx].push_back(glGetUniformLocation(_program, (std::string(lightName) + veSpotLight::DEFUALT_LIGHT_UNIFORM_OUTER_ANGLE_COS_NAME).c_str()));
				++spotLightIdx;
			}
		}
	}
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
