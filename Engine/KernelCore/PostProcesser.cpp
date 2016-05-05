#include "PostProcesser.h"
#include "SceneManager.h"
#include "FrameBufferObject.h"

vePostProcesser::vePostProcesser(veSceneManager *sm)
	: USE_VE_PTR_INIT
	, _sceneManager(sm)
{
	_renderer = new vePostProcesserRenderer;
	_renderer->setRenderStageMask(_renderer->getRenderStageMask() & ~veRenderer::LIGHTINGING);
}

vePostProcesser::~vePostProcesser()
{

}

void vePostProcesser::process(veRenderPipeline *pipeline, veFrameBufferObject *fb, veCamera *camera)
{
	auto &vp = camera->getViewport();
	veVec2 size = veVec2(vp.width - vp.x, vp.height - vp.y);
	for (unsigned int i = 0; i < _materials->getMaterialNum(); ++i) {
		auto material = _materials->getMaterial(i);
		for (unsigned int p = 0; p < material->activeTechnique()->getPassNum(); ++p) {
			auto pass = material->activeTechnique()->getPass(p);
			auto tex = pass->getTexture(vePass::AMBIENT_TEXTURE);
			tex->storage(size.x() * VE_DEVICE_PIXEL_RATIO, size.y() * VE_DEVICE_PIXEL_RATIO, 1, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, nullptr, 1);
			fb->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex);
			fb->bind(GL_COLOR_BUFFER_BIT, GL_DRAW_FRAMEBUFFER);
            pipeline->prepareForDraws(camera);
			pipeline->draw(camera, camera->getRenderQueue()->forwardRenderGroup);
			fb->unBind();
			_renderer->render(_sceneManager->getRootNode(), pass, camera);
		}
	}
}
