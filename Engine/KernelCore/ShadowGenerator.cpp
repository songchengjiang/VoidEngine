#include "ShadowGenerator.h"
#include "SceneManager.h"
#include "Light.h"

static const veMat4 LIGHT_BIAS_MAT = veMat4(0.5f, 0.0f, 0.0f, 0.5f
	, 0.0f, 0.5f, 0.0f, 0.5f
	, 0.0f, 0.0f, 0.5f, 0.5f
	, 0.0f, 0.0f, 0.0f, 1.0f);

veShadowGenerator::veShadowGenerator(veSceneManager * sm)
	: USE_VE_PTR_INIT
	, _sceneManager(sm)
{
	_shadowCamera = _sceneManager->createCamera("_SHADOW_CAMERA_");
	auto shadowFBO = veFrameBufferObjectManager::instance()->createFrameBufferObject("_SHADOW_CAMERA_FBO_");
	_shadowCamera->setFrameBufferObject(shadowFBO);
}

veShadowGenerator::~veShadowGenerator()
{

}

void veShadowGenerator::shadowing()
{
	const veLightListMap &lightListMap = _sceneManager->getLightListMap();
	veRenderer::CURRENT_RENDER_STAGE = veRenderer::DIRECTIONAL_SHADOW;
	{
		auto iter = lightListMap.find(veLight::DIRECTIONAL);
		if (iter != lightListMap.end()) {
			auto &directionalLightList = iter->second;
			for (auto &light : directionalLightList) {
				directionalLightShadowing(light.get());
			}
		}
	}

	{
		auto iter = lightListMap.find(veLight::SPOT);
		if (iter != lightListMap.end()) {
			auto &spotLightList = iter->second;
			for (auto &light : spotLightList) {
				spotLightShadowing(light.get());
			}
		}
	}

	veRenderer::CURRENT_RENDER_STAGE = veRenderer::OMNIDIRECTIONAL_SHADOW;
	{
		auto iter = lightListMap.find(veLight::POINT);
		if (iter != lightListMap.end()) {
			auto &pointLightList = iter->second;
			for (auto &light : pointLightList) {
				pointLightShadowing(light.get());
			}
		}
	}
}

void veShadowGenerator::directionalLightShadowing(veLight *light)
{
	if (!light->isShadowEnabled()) return;
	_shadowCamera->setMatrix(light->getNodeToWorldMatrix());
	_shadowCamera->setViewport({ 0, 0, int(light->getShadowResolution().x()), int(light->getShadowResolution().y()) });
	auto halfShadowArea = light->getShadowArea() * 0.5f;
	_shadowCamera->setProjectionMatrixAsOrtho(-halfShadowArea.x(), halfShadowArea.x(), -halfShadowArea.y(), halfShadowArea.y(), 0.1f, light->getAttenuationRange());
	_shadowCamera->cull();
	if (_shadowCamera->hasDynamicNodeVisibleInCamera()) {
		_shadowCamera->getFrameBufferObject()->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light->getShadowTexture());
		_shadowCamera->fillRenderQueue();
		_shadowCamera->sortRenderQueue();
		_shadowCamera->renderScene();
		_shadowCamera->clearRenderQueue();
	}
	light->setLightMatrix(LIGHT_BIAS_MAT * _shadowCamera->projectionMatrix() * _shadowCamera->viewMatrix());
}

void veShadowGenerator::pointLightShadowing(veLight *light)
{
	if (!light->isShadowEnabled()) return;
	veMat4 lightWorldMat = light->getNodeToWorldMatrix();
	veMat4 faceMats[6];
	faceMats[0] = lightWorldMat * veMat4::lookAt(veVec3::ZERO, veVec3::UNIT_X, veVec3::NEGATIVE_UNIT_Y);
	faceMats[1] = lightWorldMat * veMat4::lookAt(veVec3::ZERO, veVec3::NEGATIVE_UNIT_X, veVec3::NEGATIVE_UNIT_Y);
	faceMats[2] = lightWorldMat * veMat4::lookAt(veVec3::ZERO, veVec3::UNIT_Y, veVec3::UNIT_Z);
	faceMats[3] = lightWorldMat * veMat4::lookAt(veVec3::ZERO, veVec3::NEGATIVE_UNIT_Y, veVec3::NEGATIVE_UNIT_Z);
	faceMats[4] = lightWorldMat * veMat4::lookAt(veVec3::ZERO, veVec3::UNIT_Z, veVec3::NEGATIVE_UNIT_Y);
	faceMats[5] = lightWorldMat * veMat4::lookAt(veVec3::ZERO, veVec3::NEGATIVE_UNIT_Z, veVec3::NEGATIVE_UNIT_Y);
	_shadowCamera->setViewport({ 0, 0, int(light->getShadowResolution().x()), int(light->getShadowResolution().y()) });
	_shadowCamera->setProjectionMatrixAsPerspective(90.0f, 1.0f, 0.1f, light->getAttenuationRange());
	for (unsigned short i = 0; i < 6; ++i) {
		_shadowCamera->setMatrix(faceMats[i]);
		_shadowCamera->cull();
		if (_shadowCamera->hasDynamicNodeVisibleInCamera()) {
			_shadowCamera->getFrameBufferObject()->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, light->getShadowTexture());
			_shadowCamera->fillRenderQueue();
			_shadowCamera->sortRenderQueue();
			_shadowCamera->renderScene();
			_shadowCamera->clearRenderQueue();
		}
	}

}

void veShadowGenerator::spotLightShadowing(veLight *light)
{
	if (!light->isShadowEnabled()) return;
	_shadowCamera->setMatrix(light->getNodeToWorldMatrix());
	_shadowCamera->setViewport({ 0, 0, int(light->getShadowResolution().x()), int(light->getShadowResolution().y()) });
	_shadowCamera->setProjectionMatrixAsPerspective(2.0f * static_cast<veSpotLight *>(light)->getOuterAngle(), 1.0f, 0.1f, light->getAttenuationRange());
	_shadowCamera->cull();
	if (_shadowCamera->hasDynamicNodeVisibleInCamera()) {
		_shadowCamera->getFrameBufferObject()->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light->getShadowTexture());
		_shadowCamera->fillRenderQueue();
		_shadowCamera->sortRenderQueue();
		_shadowCamera->renderScene();
		_shadowCamera->clearRenderQueue();
	}
	light->setLightMatrix(LIGHT_BIAS_MAT * _shadowCamera->projectionMatrix() * _shadowCamera->viewMatrix());
}