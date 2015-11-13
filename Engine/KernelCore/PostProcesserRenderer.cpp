#include "PostProcesserRenderer.h"
#include "RenderQueue.h"
#include "Camera.h"
#include "Node.h"
#include "MatrixPtr.h"

vePostProcesserRenderer::vePostProcesserRenderer()
{
}

vePostProcesserRenderer::~vePostProcesserRenderer()
{

}

void vePostProcesserRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera)
{
	updateBuffer();
	if (_pass.valid()) {
		if (camera->getMask() & _pass->drawMask()) {
			veRenderCommand rc;
			rc.pass = _pass.get();
			rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix());
			//rc.attachedNode = node;
			rc.renderableObj = renderableObj;
			rc.camera = camera;
			rc.drawFunc = VE_CALLBACK_1(vePostProcesserRenderer::draw, this);
			_pass->visit(rc);
			camera->getRenderQueue()->pushCommand(veRenderQueue::RENDER_QUEUE_OVERLAY, rc);
		}
	}
}
