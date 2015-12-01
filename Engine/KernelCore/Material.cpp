#include "Material.h"
#include "FrameBufferObject.h"
#include "ShaderDefinationGenerator.h"
#include "Light.h"
#include "SceneManager.h"

veBlendFunc veBlendFunc::DISABLE = { GL_ONE, GL_ZERO };
veBlendFunc veBlendFunc::ADDITIVE = { GL_SRC_ALPHA, GL_ONE };
veBlendFunc veBlendFunc::ALPHA = { GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };

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
	, _castShadow(false)
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
	if (CURRENT_PASS == this) return true;
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
		glUseProgram(0);
		//if (_program) {
		//	glDeleteProgram(_program);
		//	_program = 0;
		//}
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
	if (lightList.empty() || _lightUniformLocations.lightNum < 0)
		return;

	unsigned int enabledLightIndex = 0;
	for (auto &iter : lightList) {
		if (iter->isInScene() && iter->isVisible() && (iter->getMask() & command.mask)) {
			applyLightUniforms(enabledLightIndex, iter.get(), command.camera);
			++enabledLightIndex;
		}
	}

	glUniform1i(_lightUniformLocations.lightNum, enabledLightIndex);
}

void vePass::applyUniforms(const veRenderCommand &command)
{
	for (auto &iter : _uniforms) {
		iter->apply(command);
	}
}

void vePass::applyLightUniforms(unsigned int idx, veLight *light, veCamera *camera)
{
	if (light->getMask() & camera->getMask()) {
		light->setLightViewMatrix(camera->viewMatrix() * light->getNodeToWorldMatrix());
	}

	const auto &lightParamLocs = _lightUniformLocations.lightParams[idx];

	glUniform1i(lightParamLocs[0], GLint(light->getLightType()));

	veMat4 lightInView = light->getLightViewMatrix();
	if (light->getLightType() == veLight::POINT || light->getLightType() == veLight::SPOT) {
		glUniform3f(lightParamLocs[1], lightInView[0][3], lightInView[1][3], lightInView[2][3]);
	}

	if (light->getLightType() == veLight::DIRECTIONAL || light->getLightType() == veLight::SPOT) {
		lightInView[0][3] = lightInView[1][3] = lightInView[2][3] = 0.0f;
		veVec3 dir = lightInView * -veVec3::UNIT_Z;
		dir.normalize();
		glUniform3f(lightParamLocs[2], dir.x(), dir.y(), dir.z());
	}

	const auto &color = light->getColor();
	glUniform3f(lightParamLocs[3], color.r(), color.g(), color.b());
	glUniform1f(lightParamLocs[4], light->getIntensity());
	glUniform1f(lightParamLocs[5], light->getAttenuationRangeInverse());

	if (light->getLightType() == veLight::SPOT) {
		glUniform1f(lightParamLocs[6], light->getInnerAngleCos());
		glUniform1f(lightParamLocs[7], light->getOuterAngleCos());
	}

	glUniform1f(lightParamLocs[8], light->isShadowEnabled()? 1.0f : 0.0f);

	if (light->isShadowEnabled()) {
		glUniform1f(lightParamLocs[9], light->getShadowBias());
		glUniform1f(lightParamLocs[10], light->getShadowStrength());
		glUniform2f(lightParamLocs[11], light->getShadowResolution().x(), light->getShadowResolution().y());

		glUniform1i(lightParamLocs[12], idx + _textures.size());
		light->getShadowTexture()->bind(idx + _textures.size());
	}
}

void vePass::locateLightUnifroms(const veRenderCommand &command)
{
	if (command.sceneManager->getLightList().empty())
		return;
	size_t lightMaxNum = command.sceneManager->getLightList().size();
	_lightUniformLocations.lightNum = glGetUniformLocation(_program, veLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME.c_str());
	_lightUniformLocations.lightParams.assign(lightMaxNum, std::vector<GLint>());
	for (unsigned int i = 0; i < lightMaxNum; ++i) {
		char str[32];
		sprintf(str, "%s[%d].", veLight::DEFUALT_LIGHT_UNIFORM_NAME.c_str(), i);
		std::string lightName = str;
		_lightUniformLocations.lightParams[i].push_back(glGetUniformLocation(_program, (lightName + veLight::DEFUALT_LIGHT_UNIFORM_TYPE_NAME).c_str()));
		_lightUniformLocations.lightParams[i].push_back(glGetUniformLocation(_program, (lightName + veLight::DEFUALT_LIGHT_UNIFORM_POSITION_NAME).c_str()));
		_lightUniformLocations.lightParams[i].push_back(glGetUniformLocation(_program, (lightName + veLight::DEFUALT_LIGHT_UNIFORM_DIRECTION_NAME).c_str()));
		_lightUniformLocations.lightParams[i].push_back(glGetUniformLocation(_program, (lightName + veLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME).c_str()));
		_lightUniformLocations.lightParams[i].push_back(glGetUniformLocation(_program, (lightName + veLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME).c_str()));
		_lightUniformLocations.lightParams[i].push_back(glGetUniformLocation(_program, (lightName + veLight::DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME).c_str()));
		_lightUniformLocations.lightParams[i].push_back(glGetUniformLocation(_program, (lightName + veLight::DEFUALT_LIGHT_UNIFORM_INNER_ANGLE_COS_NAME).c_str()));
		_lightUniformLocations.lightParams[i].push_back(glGetUniformLocation(_program, (lightName + veLight::DEFUALT_LIGHT_UNIFORM_OUTER_ANGLE_COS_NAME).c_str()));
		_lightUniformLocations.lightParams[i].push_back(glGetUniformLocation(_program, (lightName + veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME).c_str()));
		_lightUniformLocations.lightParams[i].push_back(glGetUniformLocation(_program, (lightName + veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME).c_str()));
		_lightUniformLocations.lightParams[i].push_back(glGetUniformLocation(_program, (lightName + veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME).c_str()));
		_lightUniformLocations.lightParams[i].push_back(glGetUniformLocation(_program, (lightName + veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_RESOLUTION_NAME).c_str()));
		_lightUniformLocations.lightParams[i].push_back(glGetUniformLocation(_program, (lightName + veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_NAME).c_str()));
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
