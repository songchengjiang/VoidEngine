#include "SkyBox.h"
#include "BoxRenderer.h"
#include "Camera.h"
#include "SceneManager.h"

class veSkyBoxRenderer : public veBoxRenderer
{
public:

	veSkyBoxRenderer(veSkyBox *skybox)
		: _skyBox(skybox)
	{}

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) override {
		updateBuffer();
		auto materials = _skyBox->getMaterialArray();
		for (unsigned int mat = 0; mat < materials->getMaterialNum(); ++mat) {
			auto material = materials->getMaterial(mat);
			for (unsigned int i = 0; i < material->activeTechnique()->getPassNum(); ++i) {
				auto pass = material->activeTechnique()->getPass(i);
				if (camera->getMask() & pass->drawMask()) {
					veRenderCommand rc;
					rc.pass = pass;
					rc.worldMatrix = new veMat4Ptr(veMat4::translation(veVec3(camera->getMatrix()[0][3], camera->getMatrix()[1][3], camera->getMatrix()[2][3]))
						* veMat4::scale(veVec3(_skyBox->getSize() * 0.5f)));
					rc.renderableObj = renderableObj;
					rc.camera = camera;
					rc.sceneManager = camera->getSceneManager();
					rc.drawFunc = VE_CALLBACK_1(veSkyBoxRenderer::draw, this);
					pass->visit(rc);
					camera->getRenderQueue()->pushCommand(veRenderQueue::RENDER_QUEUE_BACKGROUND, rc);
				}
			}
		}
	}

private:

	veSkyBox *_skyBox;
};

veSkyBox::veSkyBox(veReal size)
	: USE_VE_PTR_INIT
{
	_renderer = new veSkyBoxRenderer(this);
	setSize(size);
}

veSkyBox::~veSkyBox()
{

}

void veSkyBox::setSize(veReal size)
{
	_size = size;
}

void veSkyBox::render(veCamera *camera)
{
	_renderer->render(nullptr, nullptr, camera);
}
