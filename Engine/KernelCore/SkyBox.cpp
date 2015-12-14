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
		if (!isNeedRendering())
			return;
		updateBuffer();

		veMat4 cameraWorldMat = camera->getNodeToWorldMatrix();
		veRenderCommand rc;
		veMat4 mat = veMat4::IDENTITY;
		mat[0][3] = cameraWorldMat[0][3];
		mat[1][3] = cameraWorldMat[1][3];
		mat[2][3] = cameraWorldMat[2][3];
		mat[0][0] = mat[1][1] = mat[2][2] = _skyBox->getSize() * 0.5f;
		rc.worldMatrix = new veMat4Ptr(mat);
		rc.renderableObj = renderableObj;
		rc.camera = camera;
		rc.sceneManager = camera->getSceneManager();
		rc.renderer = this;

		auto materials = _skyBox->getMaterialArray();
		for (unsigned int mat = 0; mat < materials->getMaterialNum(); ++mat) {
			auto material = materials->getMaterial(mat);
			for (unsigned int i = 0; i < material->activeTechnique()->getPassNum(); ++i) {
				auto pass = material->activeTechnique()->getPass(i);
				if (camera->getMask() & pass->drawMask()) {
					rc.pass = pass;
					pass->visit(rc);
					camera->getRenderQueue()->pushCommand(i, veRenderQueue::RENDER_QUEUE_BACKGROUND, rc);
				}
			}
		}
	}

private:

	veSkyBox *_skyBox;
};

veSkyBox::veSkyBox(veReal size)
	: USE_VE_PTR_INIT
	, _mask(0xffffffff)
	, _sceneManager(nullptr)
{
	_renderer = new veSkyBoxRenderer(this);
	_renderer->setRenderStageMask(_renderer->getRenderStageMask() & ~veRenderer::SHADOWING & ~veRenderer::LIGHTINGING & ~veRenderer::DEPTH);
	setSize(size);
}

veSkyBox::~veSkyBox()
{

}

void veSkyBox::setSize(veReal size)
{
	_size = size;
}

void veSkyBox::setMask(unsigned int mask)
{
	_mask = mask;
	_sceneManager->needReload();
}

void veSkyBox::render(veCamera *camera)
{
	_renderer->render(nullptr, nullptr, camera);
}
