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

void veBillboardRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID)
{
	if (_firstUpdate) {
		_originBoundingBox = renderableObj->getBoundingBox();
		_firstUpdate = false;
	}
	updateBuffer(contextID);

	veQuat cameraRot;
	camera->getNodeToWorldMatrix().decomposition(nullptr, nullptr, &cameraRot);
	veMat4 rotMat;
	rotMat.makeRotation(cameraRot);
	renderableObj->setBoundingBox(_originBoundingBox * rotMat);
	veRenderCommand rc;
	rc.mask = node->getMask();
	rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix() * rotMat);
	//rc.attachedNode = node;
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.depthInCamera = (camera->viewMatrix() * rc.worldMatrix->value())[2][3];
	rc.renderer = this;
    rc.contextID = contextID;

    auto material = renderableObj->getMaterial();
    for (unsigned int i = 0; i < material->activeTechnique()->getPassNum(); ++i) {
        auto pass = material->activeTechnique()->getPass(i);
        if (camera->getMask() & pass->drawMask()) {
            bool isTransparent = pass->blendFunc() != veBlendFunc::DISABLE ? true : false;
            rc.pass = pass;
            //rc.drawFunc = VE_CALLBACK_1(veBillboardRenderer::draw, this);
            pass->visit(rc);
            if (isTransparent)
                camera->getRenderQueue()->pushCommand(i, veRenderQueue::RENDER_QUEUE_TRANSPARENT, rc);
            else
                camera->getRenderQueue()->pushCommand(i, veRenderQueue::RENDER_QUEUE_ENTITY, rc);
        }
    }
}
