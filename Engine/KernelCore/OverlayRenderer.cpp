#include "OverlayRenderer.h"
#include "RenderQueue.h"
#include "Camera.h"
#include "Node.h"
#include "MatrixPtr.h"

veOverlayRenderer::veOverlayRenderer()
	: _renderOrder(0)
{
}

veOverlayRenderer::~veOverlayRenderer()
{

}

void veOverlayRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera)
{
	updateBuffer();
	auto materials = renderableObj->getMaterialArray();
	for (unsigned int mat = 0; mat < materials->getMaterialNum(); ++mat) {
		auto material = materials->getMaterial(mat);
		for (unsigned int i = 0; i < material->activeTechnique()->getPassNum(); ++i) {
			auto pass = material->activeTechnique()->getPass(i);
			if (camera->getMask() & pass->drawMask()) {
				veRenderCommand rc;
				rc.priority = _renderOrder;
				rc.pass = pass;
				rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix());
				//rc.attachedNode = node;
				rc.renderableObj = renderableObj;
				rc.camera = camera;
				rc.drawFunc = VE_CALLBACK_1(veOverlayRenderer::draw, this);
				pass->visit(rc);
				camera->getRenderQueue()->pushCommand(veRenderQueue::RENDER_QUEUE_OVERLAY, rc);
			}
		}
	}
}
