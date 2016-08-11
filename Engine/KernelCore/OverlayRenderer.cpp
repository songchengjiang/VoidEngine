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

void veOverlayRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID)
{
	updateBuffer(contextID);
	veRenderCommand rc;
	rc.priority = _renderOrder;
	rc.mask = node->getMask();
	rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix());
	//rc.attachedNode = node;
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.renderer = this;
    rc.contextID = contextID;

    auto material = renderableObj->getMaterial();
    for (unsigned int i = 0; i < material->activeTechnique()->getPassNum(); ++i) {
        auto pass = material->activeTechnique()->getPass(i);
        if (camera->getMask() & pass->drawMask()) {
            rc.pass = pass;
            pass->visit(rc);
            camera->getRenderQueue()->pushCommand(i, veRenderQueue::RENDER_QUEUE_OVERLAY, rc);
        }
    }
}
