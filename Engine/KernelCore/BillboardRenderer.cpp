#include "BillboardRenderer.h"
#include "RenderQueue.h"
#include "Visualiser.h"
#include "Node.h"
#include "MatrixPtr.h"

veBillboardRenderer::veBillboardRenderer()
{
}

veBillboardRenderer::~veBillboardRenderer()
{

}

void veBillboardRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera)
{
	if (_technique) {
		updateBuffer();
		for (unsigned int i = 0; i < _technique->getPassNum(); ++i) {
			auto pass = _technique->getPass(i);
			if (camera->getMask() & pass->drawMask()) {
				bool isTransparent = pass->blendFunc() != veBlendFunc::DISABLE ? true : false;
				veQuat cameraRot;
				camera->getNodeToWorldMatrix().decomposition(nullptr, nullptr, &cameraRot);
				veRenderCommand rc;
				rc.pass = pass;
				rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix() * veMat4::rotation(cameraRot));
				//rc.attachedNode = node;
				rc.renderableObj = renderableObj;
				rc.camera = camera;
				rc.depthInCamera = (camera->viewMatrix() * rc.worldMatrix->value())[2][3];
				rc.drawFunc = VE_CALLBACK_1(veBillboardRenderer::draw, this);
				pass->visit(rc);
				if (isTransparent)
					veRenderQueue::CURRENT_RENDER_QUEUE->pushCommand(veRenderQueue::RENDER_QUEUE_TRANSPARENT, rc);
				else
					veRenderQueue::CURRENT_RENDER_QUEUE->pushCommand(veRenderQueue::RENDER_QUEUE_ENTITY, rc);
			}
		}
	}
}
