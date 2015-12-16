#include "LightRenderer.h"
#include "Node.h"
#include "Camera.h"

#define OCCLUSION_PASS 0
#define RENDERING_PASS 1

veDirectionalLightRenderer::veDirectionalLightRenderer()
{
}

veDirectionalLightRenderer::~veDirectionalLightRenderer()
{

}

void veDirectionalLightRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera)
{
	if (!isNeedRendering())
		return;
	updateBuffer();

	veRenderCommand rc;
	rc.mask = 0xffffffff;
	rc.worldMatrix = new veMat4Ptr(veMat4::IDENTITY);
	rc.renderableObj = renderableObj;
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.renderer = this;

	auto material = renderableObj->getMaterialArray()->getMaterial(0);
	rc.pass = material->activeTechnique()->getPass(0);
	camera->getRenderQueue()->pushCommand(RENDERING_PASS, veRenderQueue::RENDER_QUEUE_OVERLAY, rc);
}

vePointLightRenderer::vePointLightRenderer()
	: _lightVolumeScale(veMat4::IDENTITY)
{
}

vePointLightRenderer::~vePointLightRenderer()
{

}

void vePointLightRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera)
{
	if (!isNeedRendering())
		return;
	updateBuffer();

	veRenderCommand rc;
	rc.mask = 0xffffffff;
	rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix() * _lightVolumeScale);
	rc.renderableObj = renderableObj;
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.renderer = this;

	auto material = renderableObj->getMaterialArray()->getMaterial(0);
	rc.pass = material->activeTechnique()->getPass(0);
	camera->getRenderQueue()->pushCommand(OCCLUSION_PASS, veRenderQueue::RENDER_QUEUE_OVERLAY, rc);
	rc.pass = material->activeTechnique()->getPass(1);
	camera->getRenderQueue()->pushCommand(RENDERING_PASS, veRenderQueue::RENDER_QUEUE_OVERLAY, rc);
}

veSpotLightRenderer::veSpotLightRenderer()
{
}

veSpotLightRenderer::~veSpotLightRenderer()
{

}

void veSpotLightRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera)
{
	if (!isNeedRendering())
		return;
	updateBuffer();

	veRenderCommand rc;
	rc.mask = 0xffffffff;
	rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix() * _lightVolumeScale);
	rc.renderableObj = renderableObj;
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.renderer = this;

	auto material = renderableObj->getMaterialArray()->getMaterial(0);
	rc.pass = material->activeTechnique()->getPass(0);
	camera->getRenderQueue()->pushCommand(OCCLUSION_PASS, veRenderQueue::RENDER_QUEUE_OVERLAY, rc);
	rc.pass = material->activeTechnique()->getPass(1);
	camera->getRenderQueue()->pushCommand(RENDERING_PASS, veRenderQueue::RENDER_QUEUE_OVERLAY, rc);
}
