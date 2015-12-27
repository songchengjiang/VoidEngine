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
	if (light->_needUpdateShadowMap || light->isDynamicNode()) {
		auto &ds = _directionalShadowMapList[light];

		//texture
		if (!ds.texture.valid()) {
			ds.texture = _sceneManager->createTexture(light->getName() + std::string("-shadowTex"), veTexture::TEXTURE_2D);
			ds.texture->setTexParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			ds.texture->setTexParameter(GL_TEXTURE_COMPARE_FUNC, GL_LESS);
			light->setShadowTexture(ds.texture.get());
		}
		ds.texture->storage(int(light->getShadowResolution().x()), int(light->getShadowResolution().y()), 1
			, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr, 1);

		//camera
		if (!ds.camera.valid()) {
			ds.camera = _sceneManager->createCamera(light->getName() + std::string("-shadowCam"));
			auto shadowFBO = veFrameBufferObjectManager::instance()->createFrameBufferObject(light->getName() + std::string("-shadowFBO"));
			ds.camera->setFrameBufferObject(shadowFBO);
		}
		ds.camera->getFrameBufferObject()->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ds.texture.get());
		ds.camera->setMask(light->getMask());
		ds.camera->setMatrix(light->getNodeToWorldMatrix());
		ds.camera->setViewport({ 0, 0, int(light->getShadowResolution().x()), int(light->getShadowResolution().y()) });
		auto halfShadowArea = light->getShadowArea() * 0.5f;
		ds.camera->setProjectionMatrixAsOrtho(-halfShadowArea.x(), halfShadowArea.x(), -halfShadowArea.y(), halfShadowArea.y(), 0.1f, light->getAttenuationRange());
		ds.camera->cull();
		ds.camera->fillRenderQueue();
		ds.camera->sortRenderQueue();
		ds.camera->renderScene();
		ds.camera->clearRenderQueue();

		light->setLightMatrix(LIGHT_BIAS_MAT * ds.camera->projectionMatrix() * ds.camera->viewMatrix());
		if (light->_needUpdateShadowMap) light->_needUpdateShadowMap = false;
	}
}

void veShadowGenerator::pointLightShadowing(veLight *light)
{
	if (!light->isShadowEnabled()) return;
	if (light->_needUpdateShadowMap || light->isDynamicNode()) {
		auto &os = _omnidirectionalShadowMapList[light];

		if (!os.texture.valid()) {
			os.texture = _sceneManager->createTexture(light->getName() + std::string("-shadowTex"), veTexture::TEXTURE_CUBE);
			os.texture->setTexParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			os.texture->setTexParameter(GL_TEXTURE_COMPARE_FUNC, GL_LESS);
			light->setShadowTexture(os.texture.get());
		}
		os.texture->storage(int(light->getShadowResolution().x()), int(light->getShadowResolution().y()), 1
			, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr, 1);

		
		if (!os.cameras[0].valid()) {
			auto shadowFBO = veFrameBufferObjectManager::instance()->createFrameBufferObject(light->getName() + std::string("-shadowFBO"));
			for (unsigned short i = 0; i < 6; ++i) {
				os.cameras[i] = _sceneManager->createCamera(light->getName() + std::string("-shadowCam"));
				os.cameras[i]->setFrameBufferObject(shadowFBO);
			}
		}

		veMat4 lightWorldMat = light->getNodeToWorldMatrix();
		veVec3 lightWorldPos = veVec3(lightWorldMat[0][3], lightWorldMat[1][3], lightWorldMat[2][3]);
		veMat4 faceMats[6];
		faceMats[0] = veMat4::lookAt(lightWorldPos, lightWorldPos + veVec3::UNIT_X, veVec3::NEGATIVE_UNIT_Y);
		faceMats[1] = veMat4::lookAt(lightWorldPos, lightWorldPos + veVec3::NEGATIVE_UNIT_X, veVec3::NEGATIVE_UNIT_Y);
		faceMats[2] = veMat4::lookAt(lightWorldPos, lightWorldPos + veVec3::UNIT_Y, veVec3::UNIT_Z);
		faceMats[3] = veMat4::lookAt(lightWorldPos, lightWorldPos + veVec3::NEGATIVE_UNIT_Y, veVec3::NEGATIVE_UNIT_Z);
		faceMats[4] = veMat4::lookAt(lightWorldPos, lightWorldPos + veVec3::UNIT_Z, veVec3::NEGATIVE_UNIT_Y);
		faceMats[5] = veMat4::lookAt(lightWorldPos, lightWorldPos + veVec3::NEGATIVE_UNIT_Z, veVec3::NEGATIVE_UNIT_Y);
		for (unsigned short i = 0; i < 6; ++i) {
			os.cameras[i]->setMask(light->getMask());
			os.cameras[i]->setViewport({ 0, 0, int(light->getShadowResolution().x()), int(light->getShadowResolution().y()) });
			os.cameras[i]->setProjectionMatrixAsPerspective(90.0f, 1.0f, 0.1f, light->getAttenuationRange());
			os.cameras[i]->setMatrix(faceMats[i]);
			os.cameras[i]->cull();
			os.cameras[i]->getFrameBufferObject()->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, os.texture.get());
			os.cameras[i]->fillRenderQueue();
			os.cameras[i]->sortRenderQueue();
			os.cameras[i]->renderScene();
			os.cameras[i]->clearRenderQueue();
		}
		light->setLightMatrix(veMat4::translation(-lightWorldPos));
		if (light->_needUpdateShadowMap) light->_needUpdateShadowMap = false;
	}
}

void veShadowGenerator::spotLightShadowing(veLight *light)
{
	if (!light->isShadowEnabled()) return;
	if (light->_needUpdateShadowMap || light->isDynamicNode()) {
		auto &ds = _directionalShadowMapList[light];

		//texture
		if (!ds.texture.valid()) {
			ds.texture = _sceneManager->createTexture(light->getName() + std::string("-shadowTex"), veTexture::TEXTURE_2D);
			ds.texture->setTexParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			ds.texture->setTexParameter(GL_TEXTURE_COMPARE_FUNC, GL_LESS);
			light->setShadowTexture(ds.texture.get());
		}
		ds.texture->storage(int(light->getShadowResolution().x()), int(light->getShadowResolution().y()), 1
			, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr, 1);
		//camera
		if (!ds.camera.valid()) {
			ds.camera = _sceneManager->createCamera(light->getName() + std::string("-shadowCam"));
			auto shadowFBO = veFrameBufferObjectManager::instance()->createFrameBufferObject(light->getName() + std::string("-shadowFBO"));
			ds.camera->setFrameBufferObject(shadowFBO);
		}
		ds.camera->getFrameBufferObject()->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ds.texture.get());
		ds.camera->setMask(light->getMask());
		ds.camera->setMatrix(light->getNodeToWorldMatrix());
		ds.camera->setViewport({ 0, 0, int(light->getShadowResolution().x()), int(light->getShadowResolution().y()) });
		ds.camera->setProjectionMatrixAsPerspective(2.0f * static_cast<veSpotLight *>(light)->getOuterAngle(), 1.0f, 0.1f, light->getAttenuationRange());
		ds.camera->cull();
		ds.camera->fillRenderQueue();
		ds.camera->sortRenderQueue();
		ds.camera->renderScene();
		ds.camera->clearRenderQueue();

		light->setLightMatrix(LIGHT_BIAS_MAT * ds.camera->projectionMatrix() * ds.camera->viewMatrix());
		if (light->_needUpdateShadowMap) light->_needUpdateShadowMap = false;
	}
}