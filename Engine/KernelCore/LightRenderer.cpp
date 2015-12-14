#include "LightRenderer.h"
#include "Node.h"
#include "Camera.h"

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
	rc.mask = node->getMask();
	rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix());
	//rc.attachedNode = node;
	rc.renderableObj = renderableObj;
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.renderer = this;

	auto materials = renderableObj->getMaterialArray();
	for (unsigned int mat = 0; mat < materials->getMaterialNum(); ++mat) {
		auto material = materials->getMaterial(mat);
		for (unsigned int i = 0; i < material->activeTechnique()->getPassNum(); ++i) {
			auto pass = material->activeTechnique()->getPass(i);
			if (camera->getMask() & pass->drawMask()) {
				rc.pass = pass;
				pass->visit(rc);
				camera->getRenderQueue()->pushCommand(i, veRenderQueue::RENDER_QUEUE_OVERLAY, rc);
			}
		}
	}
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
	rc.mask = node->getMask();
	rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix() * _lightVolumeScale);
	//rc.attachedNode = node;
	rc.renderableObj = renderableObj;
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.renderer = this;

	auto materials = renderableObj->getMaterialArray();
	for (unsigned int mat = 0; mat < materials->getMaterialNum(); ++mat) {
		auto material = materials->getMaterial(mat);
		for (unsigned int i = 0; i < material->activeTechnique()->getPassNum(); ++i) {
			auto pass = material->activeTechnique()->getPass(i);
			if (camera->getMask() & pass->drawMask()) {
				rc.pass = pass;
				camera->getRenderQueue()->pushCommand(i, veRenderQueue::RENDER_QUEUE_OVERLAY, rc);
			}
		}
	}
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
	rc.mask = node->getMask();
	rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix());
	//rc.attachedNode = node;
	rc.renderableObj = renderableObj;
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.renderer = this;

	auto materials = renderableObj->getMaterialArray();
	for (unsigned int mat = 0; mat < materials->getMaterialNum(); ++mat) {
		auto material = materials->getMaterial(mat);
		for (unsigned int i = 0; i < material->activeTechnique()->getPassNum(); ++i) {
			auto pass = material->activeTechnique()->getPass(i);
			if (camera->getMask() & pass->drawMask()) {
				rc.pass = pass;
				pass->visit(rc);
				camera->getRenderQueue()->pushCommand(i, veRenderQueue::RENDER_QUEUE_OVERLAY, rc);
			}
		}
	}
}
