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
	if (command.camera->isRenderStateChanged())
		_needLinkProgram = true;
}

void vePass::apply(const veRenderCommand &command)
{
	applyProgram(command);
	applyLightsUniforms(command);
	applyUniforms(command);
	if (CURRENT_PASS == this) return;
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
	if (_textures.empty())
		_needLinkProgram = true;
	_textures.push_back(texture);
}

void vePass::setTexture(size_t idx, veTexture *texture)
{
	veAssert(idx < _textures.size());
	_textures[idx] = texture;
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
	if (_textures.empty())
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
	if (!_program)
		_program = glCreateProgram();
	if (_needLinkProgram) {
		ShaderDefinatiosGenerator sdg(command);
		//sdg.traversalMode() = ShaderDefinatiosGenerator::TRAVERSE_PARENT;
		//command.attachedNode->accept(sdg);
		//sdg.traversalMode() = ShaderDefinatiosGenerator::TRAVERSE_CHILDREN;
		//sdg.getRoot()->accept(sdg);
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
		_needLinkProgram = false;
	}
	glUseProgram(_program);
}

void vePass::applyLightsUniforms(const veRenderCommand &command)
{
	const veLightList &lightList = command.sceneManager->getLightList();
	if (lightList.empty()) return;
	std::unordered_map<std::string, int> currentLights;
	for (auto &iter : static_cast<veLightManager *>(command.sceneManager->getManager(veLightManager::TYPE()))->getLightTemplateList()) {
		currentLights[iter.first] = 0;
	}
	for (auto &iter : lightList) {
		if (iter->isInScene()) {
			int &count = currentLights[iter->getType()];
			applyLightUniforms(count, iter.get(), command.camera);
			++count;
		}
	}
	for (auto &iter : currentLights) {
		GLint loc = glGetUniformLocation(_program, (iter.first + std::string("Number")).c_str());
		glUniform1i(loc, iter.second);
	}
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

	char str[32];
	sprintf(str, "%s[%d].", light->getType().c_str(), idx);
	std::string lightName = str;

	GLint posLoc = glGetUniformLocation(_program, (lightName + POSITION_KEY).c_str());
	GLint dirLoc = glGetUniformLocation(_program, (lightName + DIRECTION_KEY).c_str());
	if (0 <= posLoc || 0 <= dirLoc) {
		veMat4 lightInView = light->getLightViewMatrix();
		if (0 <= posLoc) {
			glUniform3f(posLoc, lightInView[0][3], lightInView[1][3], lightInView[2][3]);
		}

		if (0 <= dirLoc) {
			lightInView[0][3] = lightInView[1][3] = lightInView[2][3] = 0.0f;
			veVec3 dir = lightInView * -veVec3::UNIT_Z;
			dir.normalize();
			glUniform3f(dirLoc, dir.x(), dir.y(), dir.z());
		}
	}

	for (auto &param : light->getParameters()) {
		GLint loc = glGetUniformLocation(_program, (lightName + param->getName()).c_str());
		if (loc < 0) continue;
		switch (param->getType())
		{
		case veParameter::Type::INT:
		{
			int val;
			param->get(val);
			glUniform1i(loc, val);
		}
			break;

		case veParameter::Type::BOOL:
		{
			bool val;
			param->get(val);
			glUniform1i(loc, val);
		}
			break;

		case veParameter::Type::REAL:
		{
			veReal val;
			param->get(val);
			glUniform1f(loc, val);
		}
			break;

		case veParameter::Type::VEC2:
		{
			veVec2 val;
			param->get(val);
			glUniform2f(loc, val.x(), val.y());
		}
			break;

		case veParameter::Type::VEC3:
		{
			veVec3 val;
			param->get(val);
			glUniform3f(loc, val.x(), val.y(), val.z());
		}
			break;

		case veParameter::Type::VEC4:
		{
			veVec4 val;
			param->get(val);
			glUniform4f(loc, val.x(), val.y(), val.z(), val.w());
		}
			break;
		default:
			break;
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
