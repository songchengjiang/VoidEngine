#include "DeferredLightIlluminator.h"
#include "SceneManager.h"
#include "TextureManager.h"
#include "Camera.h"
#include "Constants.h"

static const std::string DEFAULT_DEFERRED_LIGHT_MATERIALS = "_DEFAULT_DEFERRED_LIGHT_MATERIALS_";
static const std::string DEFAULT_DEFERRED_LIGHT_FBO = "_DEFAULT_DEFERRED_LIGHT_FBO_";
static const std::string DEFAULT_DEFERRED_LIGHT_SCENE_NORMAL_TEXTURE = "_DEFAULT_DEFERRED_LIGHT_SCENE_NORMAL_TEXTURE_";
static const std::string DEFAULT_DEFERRED_LIGHT_SCENE_DEPTH_TEXTURE  = "_DEFAULT_DEFERRED_LIGHT_SCENE_DEPTH_TEXTURE_";
static const std::string DEFAULT_DEFERRED_LIGHT_ILLUMINATION_TEXTURE = "_DEFAULT_DEFERRED_LIGHT_ILLUMINATION_TEXTURE_";

veDeferredLightSceneIlluminator::veDeferredLightSceneIlluminator(veCamera *camera)
	: USE_VE_PTR_INIT
	, _camera(camera)
{
	initIlluminationParams();
	initLightMaterials();
}

veDeferredLightSceneIlluminator::~veDeferredLightSceneIlluminator()
{

}

void veDeferredLightSceneIlluminator::initPassCommanParams(vePass *pass)
{
	pass->addUniform(new veUniform("u_InvViewProjectMat", INV_VP_MATRIX));
	pass->addUniform(new veUniform("u_screenWidth", SCREEN_WIDTH));
	pass->addUniform(new veUniform("u_screenHeight", SCREEN_HEIGHT));
	pass->addUniform(new veUniform("u_depthTex", 0));
	pass->addUniform(new veUniform("u_normalTex", 1));
	pass->addUniform(new veUniform("u_shadowTex", 2));
	pass->addUniform(new veUniform("u_cameraPosInWorld", veVec3(0.0f)));
	pass->addUniform(new veUniform("u_lightColor", veVec3(0.0f)));
	pass->addUniform(new veUniform("u_lightIntensity", 0.0f));
	pass->addUniform(new veUniform("u_lightMatrix", veMat4::IDENTITY));
	pass->addUniform(new veUniform("u_lightShadowEnabled", 0.0f));
	pass->addUniform(new veUniform("u_lightShadowBias", 0.0f));
	pass->addUniform(new veUniform("u_lightShadowStrength", 0.0f));
	pass->addUniform(new veUniform("u_lightShadowSoft", 0.0f));
	pass->addUniform(new veUniform("u_lightShadowSoftness", 0.0f));
	pass->addTexture(vePass::AMBIENT_TEXTURE, _sceneDepthTexture.get());
	pass->addTexture(vePass::DIFFUSE_TEXTURE, _sceneNormalTexture.get());
}

void veDeferredLightSceneIlluminator::initLightMaterials()
{
	_lightMatrials = _camera->getSceneManager()->createMaterialArray(DEFAULT_DEFERRED_LIGHT_MATERIALS);

	//directional light material
	{
		auto material = new veMaterial;
		_lightMatrials->addMaterial(material);
		auto technique = new veTechnique;
		material->addTechnique(technique);
		auto pass = new vePass;
		technique->addPass(pass);
		pass->depthTest() = false;
		pass->depthWrite() = false;
		pass->stencilTest() = false;
		pass->cullFace() = true;
		pass->cullFaceMode() = GL_BACK;
		pass->blendFunc().src = GL_ONE; pass->blendFunc().dst = GL_ONE;
		pass->blendEquation() = GL_FUNC_ADD;
		pass->setShader(new veShader(veShader::VERTEX_SHADER, std::string("system/directionalLightingPass.vert")));
		pass->setShader(new veShader(veShader::FRAGMENT_SHADER, std::string("system/directionalLightingPass.frag")));
		pass->addUniform(new veUniform("u_lightDirection", veVec3(0.0f, 0.0f, 0.0f)));
		initPassCommanParams(pass);
	}

	//point light material
	{
		auto material = new veMaterial;
		_lightMatrials->addMaterial(material);
		auto technique = new veTechnique;
		material->addTechnique(technique);

		//occlusion pass 
		{
			auto pass = new vePass;
			technique->addPass(pass);
			pass->depthTest() = true;
			pass->depthWrite() = false;
			pass->stencilTest() = true;
			pass->cullFace() = false;
			pass->stencilFunc() = { GL_ALWAYS, 0, 0, GL_ALWAYS, 0, 0 };
			pass->stencilOp() = { GL_KEEP, GL_DECR_WRAP, GL_KEEP, GL_KEEP, GL_INCR_WRAP, GL_KEEP };
			pass->blendFunc() = veBlendFunc::DISABLE;
			pass->setShader(new veShader(veShader::VERTEX_SHADER, std::string("system/pointLightingPass.vert")));
			pass->setShader(new veShader(veShader::FRAGMENT_SHADER, "layout(location=0) out vec4 fragColor;void main(){}"));
			pass->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
		}

		//rendering pass
		{
			auto pass = new vePass;
			technique->addPass(pass);
			pass->depthTest() = false;
			pass->depthWrite() = false;
			pass->stencilTest() = true;
			pass->cullFace() = true;
			pass->cullFaceMode() = GL_FRONT;
			pass->blendFunc().src = GL_ONE; pass->blendFunc().dst = GL_ONE;
			pass->blendEquation() = GL_FUNC_ADD;
			pass->stencilFunc() = { GL_NOTEQUAL, 0, 0xFF, GL_NOTEQUAL, 0, 0xFF };
			pass->setShader(new veShader(veShader::VERTEX_SHADER, std::string("system/pointLightingPass.vert")));
			pass->setShader(new veShader(veShader::FRAGMENT_SHADER, std::string("system/pointLightingPass.frag")));
			pass->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
			pass->addUniform(new veUniform("u_lightPosition", veVec3(0.0f)));
			pass->addUniform(new veUniform("u_lightARI", 0.0f));
			initPassCommanParams(pass);
		}
	}

	//spot light material
	{
		auto material = new veMaterial;
		_lightMatrials->addMaterial(material);
		auto technique = new veTechnique;
		material->addTechnique(technique);

		//occlusion pass 
		{
			auto pass = new vePass;
			technique->addPass(pass);
			pass->depthTest() = true;
			pass->depthWrite() = false;
			pass->stencilTest() = true;
			pass->cullFace() = false;
			pass->stencilFunc() = { GL_ALWAYS, 0, 0, GL_ALWAYS, 0, 0 };
			pass->stencilOp() = { GL_KEEP, GL_DECR_WRAP, GL_KEEP, GL_KEEP, GL_INCR_WRAP, GL_KEEP };
			pass->blendFunc() = veBlendFunc::DISABLE;
			pass->setShader(new veShader(veShader::VERTEX_SHADER, std::string("system/spotLightingPass.vert")));
			pass->setShader(new veShader(veShader::FRAGMENT_SHADER, "layout(location=0) out vec4 fragColor;void main(){}"));
			pass->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
		}

		//rendering pass
		{
			auto pass = new vePass;
			technique->addPass(pass);
			pass->depthTest() = false;
			pass->depthWrite() = false;
			pass->stencilTest() = true;
			pass->cullFace() = true;
			pass->cullFaceMode() = GL_FRONT;
			pass->blendFunc().src = GL_ONE; pass->blendFunc().dst = GL_ONE;
			pass->blendEquation() = GL_FUNC_ADD;
			pass->stencilFunc() = { GL_NOTEQUAL, 0, 0xFF, GL_NOTEQUAL, 0, 0xFF };
			pass->setShader(new veShader(veShader::VERTEX_SHADER, std::string("system/spotLightingPass.vert")));
			pass->setShader(new veShader(veShader::FRAGMENT_SHADER, std::string("system/spotLightingPass.frag")));
			pass->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
			pass->addUniform(new veUniform("u_lightDirection", veVec3(0.0f)));
			pass->addUniform(new veUniform("u_lightPosition", veVec3(0.0f)));
			pass->addUniform(new veUniform("u_lightARI", 0.0f));
			pass->addUniform(new veUniform("u_lightInnerAngleCos", 0.0f));
			pass->addUniform(new veUniform("u_lightOuterAngleCos", 0.0f));
			initPassCommanParams(pass);
		}
	}

}

void veDeferredLightSceneIlluminator::initIlluminationParams()
{
	_illuminationFBO = veFrameBufferObjectManager::instance()->createFrameBufferObject(DEFAULT_DEFERRED_LIGHT_FBO);
	_sceneDepthTexture = _camera->getSceneManager()->createTexture(DEFAULT_DEFERRED_LIGHT_SCENE_DEPTH_TEXTURE, veTexture::TEXTURE_2D);
	_sceneNormalTexture = _camera->getSceneManager()->createTexture(DEFAULT_DEFERRED_LIGHT_SCENE_NORMAL_TEXTURE, veTexture::TEXTURE_2D);
	_illuminationTexture = _camera->getSceneManager()->createTexture(DEFAULT_DEFERRED_LIGHT_ILLUMINATION_TEXTURE, veTexture::TEXTURE_2D);
	_illuminationFBO->attach(GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _sceneDepthTexture.get());
	//_illuminationFBO->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _sceneNormalTexture.get());
	//_illuminationFBO->attach(GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _illuminationTexture.get());
	_sceneDepthTexture->setFilterMode(veTexture::NEAREST);
	_sceneNormalTexture->setFilterMode(veTexture::NEAREST);
	_illuminationTexture->setFilterMode(veTexture::NEAREST);

	_directionalLightRenderer = new veScreenLightRenderer;
	_pointLightRenderer = new veSphereLightRenderer;
	_spotLightRenderer = new veConeLightRenderer;
}

void veDeferredLightSceneIlluminator::illuminate()
{
	const veLightListMap &lightListMap = _camera->getSceneManager()->getLightListMap();
	_illuminationFBO->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _sceneNormalTexture.get());
	_illuminationFBO->bind(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glViewport(0, 0, GLsizei(_size.x()) * VE_DEVICE_PIXEL_RATIO, GLsizei(_size.y()) * VE_DEVICE_PIXEL_RATIO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	_camera->renderRenderQueue();

	_illuminationFBO->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nullptr);
	_illuminationFBO->bind(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	{
		auto iter = lightListMap.find(veLight::POINT);
		if (iter != lightListMap.end()) {
			auto &pointLightList = iter->second;
			for (auto &light : pointLightList) {
				pointLightCulling(light.get());
			}
		}
	}

	{
		auto iter = lightListMap.find(veLight::SPOT);
		if (iter != lightListMap.end()) {
			auto &spotLightList = iter->second;
			for (auto &light : spotLightList) {
				spotLightCulling(light.get());
			}
		}
	}

	_illuminationFBO->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _illuminationTexture.get());
	_illuminationFBO->bind(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	{
		auto iter = lightListMap.find(veLight::DIRECTIONAL);
		if (iter != lightListMap.end()) {
			auto &directionalLightList = iter->second;
			for (auto &light : directionalLightList) {
				directionalLightIlluminate(light.get());
			}
		}
	}

	{
		auto iter = lightListMap.find(veLight::POINT);
		if (iter != lightListMap.end()) {
			auto &pointLightList = iter->second;
			for (auto &light : pointLightList) {
				pointLightIlluminate(light.get());
			}
		}
	}

	{
		auto iter = lightListMap.find(veLight::SPOT);
		if (iter != lightListMap.end()) {
			auto &spotLightList = iter->second;
			for (auto &light : spotLightList) {
				spotLightIlluminate(light.get());
			}
		}
	}

	_illuminationFBO->unBind();
}

void veDeferredLightSceneIlluminator::resize(const veVec2 &size)
{
	if (_size == size) return;
	_size = size;
	_illuminationFBO->setFrameBufferSize(size);
	_sceneDepthTexture->storage(size.x(), size.y(), 1, GL_DEPTH24_STENCIL8, GL_RGBA, GL_FLOAT, nullptr, 1);
#if (VE_PLATFORM == VE_PLATFORM_WIN32 || VE_PLATFORM == VE_PLATFORM_MAC)
	_sceneNormalTexture->storage(size.x(), size.y(), 1, GL_RGBA32F, GL_RGBA, GL_FLOAT, nullptr, 1);
	_illuminationTexture->storage(size.x(), size.y(), 1, GL_RGBA32F, GL_RGBA, GL_FLOAT, nullptr, 1);
#elif (VE_PLATFORM == VE_PLATFORM_ANDROID || VE_PLATFORM == VE_PLATFORM_IOS)
	_sceneNormalTexture->storage(size.x(), size.y(), 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, 1);
	_illuminationTexture->storage(size.x(), size.y(), 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, 1);
#endif
}

void veDeferredLightSceneIlluminator::fillCommonLightParamsToPass(veLight *light, vePass *pass)
{
	veMat4 cameraWorldMat = _camera->getNodeToWorldMatrix();
	pass->getUniform("u_cameraPosInWorld")->setValue(veVec3(cameraWorldMat[0][3], cameraWorldMat[1][3], cameraWorldMat[2][3]));
	pass->getUniform("u_lightColor")->setValue(light->getColor());
	pass->getUniform("u_lightIntensity")->setValue(light->getIntensity());
	pass->getUniform("u_lightMatrix")->setValue(light->getLightMatrix());
	pass->getUniform("u_lightShadowEnabled")->setValue(light->isShadowEnabled()? 1.0f: 0.0f);
	pass->getUniform("u_lightShadowBias")->setValue(light->getShadowBias());
	pass->getUniform("u_lightShadowStrength")->setValue(light->getShadowStrength());
	pass->getUniform("u_lightShadowSoft")->setValue(light->isUseSoftShadow() ? 1.0f: 0.0f);
	pass->getUniform("u_lightShadowSoftness")->setValue(light->getShadowSoftness());
	pass->addTexture(vePass::OPACITYT_TEXTURE, light->getShadowTexture());
}

void veDeferredLightSceneIlluminator::directionalLightIlluminate(veLight *light)
{
	vePass *pass = _lightMatrials->getMaterial(0)->getTechnique(0)->getPass(0);
	fillCommonLightParamsToPass(light, pass);

	veMat4 lightInWorld = light->getNodeToWorldMatrix();
	lightInWorld[0][3] = lightInWorld[1][3] = lightInWorld[2][3] = 0.0f;
	veVec3 direction = lightInWorld * -veVec3::UNIT_Z;
	direction.normalize();
	pass->getUniform("u_lightDirection")->setValue(direction);
	_directionalLightRenderer->immediatelyRender(light, pass, _camera);
}

void veDeferredLightSceneIlluminator::pointLightCulling(veLight *light)
{
	vePass *pass = _lightMatrials->getMaterial(1)->getTechnique(0)->getPass(0);
	_pointLightRenderer->setLightVolumeScale(veMat4::scale(veVec3(light->getAttenuationRange())));
	_pointLightRenderer->immediatelyRender(light, pass, _camera);
}

void veDeferredLightSceneIlluminator::pointLightIlluminate(veLight *light)
{
	vePass *pass = _lightMatrials->getMaterial(1)->getTechnique(0)->getPass(1);
	fillCommonLightParamsToPass(light, pass);

	vePointLight *pointLight = static_cast<vePointLight *>(light);
	veMat4 lightInWorld = pointLight->getNodeToWorldMatrix();
	pass->getUniform("u_lightPosition")->setValue(veVec3(lightInWorld[0][3], lightInWorld[1][3], lightInWorld[2][3]));
	pass->getUniform("u_lightARI")->setValue(pointLight->getAttenuationRangeInverse());
	_pointLightRenderer->setLightVolumeScale(veMat4::scale(veVec3(pointLight->getAttenuationRange())));
	_pointLightRenderer->immediatelyRender(pointLight, pass, _camera);
}

void veDeferredLightSceneIlluminator::spotLightCulling(veLight *light)
{
	vePass *pass = _lightMatrials->getMaterial(2)->getTechnique(0)->getPass(0);
	veSpotLight *spotLight = static_cast<veSpotLight *>(light);
	float rangeScale = spotLight->getAttenuationRange() * spotLight->getOuterAngle() / 45.0f;
	_spotLightRenderer->setLightVolumeScale(veMat4::scale(veVec3(rangeScale, rangeScale, spotLight->getAttenuationRange())));
	_spotLightRenderer->immediatelyRender(spotLight, pass, _camera);
}

void veDeferredLightSceneIlluminator::spotLightIlluminate(veLight *light)
{
	vePass *pass = _lightMatrials->getMaterial(2)->getTechnique(0)->getPass(1);
	fillCommonLightParamsToPass(light, pass);

	veSpotLight *spotLight = static_cast<veSpotLight *>(light);
	veMat4 lightInWorld = spotLight->getNodeToWorldMatrix();
	pass->getUniform("u_lightPosition")->setValue(veVec3(lightInWorld[0][3], lightInWorld[1][3], lightInWorld[2][3]));
	lightInWorld[0][3] = lightInWorld[1][3] = lightInWorld[2][3] = 0.0f;
	veVec3 direction = lightInWorld * -veVec3::UNIT_Z;
	direction.normalize();
	pass->getUniform("u_lightDirection")->setValue(direction);

	pass->getUniform("u_lightARI")->setValue(spotLight->getAttenuationRangeInverse());
	pass->getUniform("u_lightInnerAngleCos")->setValue(spotLight->getInnerAngleCos());
	pass->getUniform("u_lightOuterAngleCos")->setValue(spotLight->getOuterAngleCos());
	float rangeScale = spotLight->getAttenuationRange() * spotLight->getOuterAngle() / 45.0f;
	_spotLightRenderer->setLightVolumeScale(veMat4::scale(veVec3(rangeScale, rangeScale, spotLight->getAttenuationRange())));
	_spotLightRenderer->immediatelyRender(spotLight, pass, _camera);
}