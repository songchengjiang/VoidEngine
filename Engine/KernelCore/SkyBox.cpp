#include "SkyBox.h"
#include "BoxRenderer.h"
#include "Camera.h"
#include "SceneManager.h"

class veSkyBoxRenderer : public veBoxRenderer
{
public:

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) override {
		if (_technique) {
			auto skybox = static_cast<veSkyBox *>(renderableObj);
			updateBuffer();
			for (unsigned int i = 0; i < _technique->getPassNum(); ++i) {
				auto pass = _technique->getPass(i);
				if (camera->getMask() & pass->drawMask()) {
					veRenderCommand rc;
					rc.pass = pass;
					rc.worldMatrix = new veMat4Ptr(veMat4::translation(veVec3(camera->getMatrix()[0][3], camera->getMatrix()[1][3], camera->getMatrix()[2][3]))
						* veMat4::scale(veVec3(skybox->getSize() * 0.5f)));
					//rc.attachedNode = node;
					rc.renderableObj = renderableObj;
					rc.camera = camera;
					rc.drawFunc = VE_CALLBACK_1(veSkyBoxRenderer::draw, this);
					pass->visit(rc);
					veRenderQueue::CURRENT_RENDER_QUEUE->pushCommand(veRenderQueue::RENDER_QUEUE_BACKGROUND, rc);
				}
			}
		}
	}
};

veSkyBox::veSkyBox(veReal size)
{
	_renderer = new veSkyBoxRenderer;
	setSize(size);
}

veSkyBox::~veSkyBox()
{

}

void veSkyBox::setSize(veReal size)
{
	_size = size;
	_boundingBox.min() = veVec3(-_size * 0.5f);
	_boundingBox.max() = veVec3(_size * 0.5f);
}
