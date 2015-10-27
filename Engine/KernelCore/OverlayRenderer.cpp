#include "OverlayRenderer.h"
#include "RenderQueue.h"
#include "Visualiser.h"
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
	if (_technique) {
		updateBuffer();
		for (unsigned int i = 0; i < _technique->getPassNum(); ++i) {
			auto pass = _technique->getPass(i);
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
