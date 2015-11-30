#include "BillboardRenderer.h"
#include "RenderQueue.h"
#include "Camera.h"
#include "Node.h"
#include "MatrixPtr.h"

veBillboardRenderer::veBillboardRenderer()
	: _firstUpdate(true)
{
}

veBillboardRenderer::~veBillboardRenderer()
{

}

void veBillboardRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera)
{
	if (_firstUpdate) {
		_originBoundingBox = renderableObj->getBoundingBox();
		_firstUpdate = false;
	}
	updateBuffer();
	auto materials = renderableObj->getMaterialArray();
	for (unsigned int mat = 0; mat < materials->getMaterialNum(); ++mat) {
		auto material = materials->getMaterial(mat);
		for (unsigned int i = 0; i < material->activeTechnique()->getPassNum(); ++i) {
			auto pass = material->activeTechnique()->getPass(i);
			if (camera->getMask() & pass->drawMask()) {
				bool isTransparent = pass->blendFunc() != veBlendFunc::DISABLE ? true : false;
				veQuat cameraRot;
				camera->getNodeToWorldMatrix().decomposition(nullptr, nullptr, &cameraRot);
				veMat4 rotMat;
				rotMat.makeRotation(cameraRot);
				renderableObj->setBoundingBox(_originBoundingBox * rotMat);
				veRenderCommand rc;
				rc.mask = node->getMask();
				rc.pass = pass;
				rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix() * rotMat);
				//rc.attachedNode = node;
				rc.renderableObj = renderableObj;
				rc.camera = camera;
				rc.sceneManager = camera->getSceneManager();
				rc.depthInCamera = (camera->viewMatrix() * rc.worldMatrix->value())[2][3];
				rc.renderer = this;
				//rc.drawFunc = VE_CALLBACK_1(veBillboardRenderer::draw, this);
				pass->visit(rc);
				if (isTransparent)
					camera->getRenderQueue()->pushCommand(veRenderQueue::RENDER_QUEUE_TRANSPARENT, rc);
				else
					camera->getRenderQueue()->pushCommand(veRenderQueue::RENDER_QUEUE_ENTITY, rc);
			}
		}
	}
}
