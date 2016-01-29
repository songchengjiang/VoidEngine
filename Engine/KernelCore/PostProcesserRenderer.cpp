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

void vePostProcesserRenderer::render(veNode *node, vePass *pass, veCamera *camera)
{
	updateBuffer();
	if (pass) {
		if (camera->getMask() & pass->drawMask()) {
			veRenderCommand rc;
			rc.mask = node->getMask();
			rc.pass = pass;
			rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix());
			rc.camera = camera;
			rc.sceneManager = camera->getSceneManager();
			rc.renderer = this;
			pass->visit(rc);
			camera->getRenderQueue()->pushCommand(0, veRenderQueue::RENDER_QUEUE_OVERLAY, rc);
		}
	}
}