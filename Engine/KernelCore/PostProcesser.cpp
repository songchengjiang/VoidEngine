#include "PostProcesser.h"
#include "SceneManager.h"
#include "FrameBufferObject.h"
#include "Configuration.h"

vePostProcesser::vePostProcesser(veSceneManager *sm)
	: USE_VE_PTR_INIT
    , _isEnabled(true)
	, _sceneManager(sm)
{
	_renderer = new vePostProcesserRenderer;
	_renderer->setRenderStageMask(_renderer->getRenderStageMask() & ~veRenderer::LIGHTINGING);
}

vePostProcesser::~vePostProcesser()
{

}

void vePostProcesser::process(veRenderPipeline *pipeline, veCamera *camera, bool firstProcesser, unsigned int contextID)
{
    auto &vp = camera->getViewport();
    veVec2 size = veVec2(vp.width - vp.x, vp.height - vp.y);
    if (!_fbo.valid()) {
        _fbo = _sceneManager->createFrameBufferObject(_name + std::string("_VE_DEFERRED_RENDER_PIPELINE_POST_PROCESSER_FBO_"));
    }
    _fbo->setFrameBufferSize(size);
	for (unsigned int i = 0; i < _materials->getMaterialNum(); ++i) {
		auto material = _materials->getMaterial(i);
		for (unsigned int p = 0; p < material->activeTechnique()->getPassNum(); ++p) {
			auto pass = material->activeTechnique()->getPass(p);
			auto colTex = pass->getTexture(vePass::COLOR_BUFFER_TEXTURE);
            if (colTex) {
				unsigned int bufferMask = firstProcesser ? GL_DEPTH_BUFFER_BIT : 0;
                colTex->storage(size.x(), size.y(), 1, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, nullptr, 1);
                _fbo->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colTex);
                auto depTex = pass->getTexture(vePass::DEPTH_STENCIL_BUFFER_TEXTURE);
                if (depTex) {
                    depTex->storage(size.x(), size.y(), 1, GL_DEPTH24_STENCIL8, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr, 1);
                    _fbo->attach(GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depTex);
					bufferMask = GL_DEPTH_BUFFER_BIT;
                }
                
                _fbo->bind(contextID, camera->getClearMask(), GL_DRAW_FRAMEBUFFER);
                pipeline->renderToPostProcesser(this, camera, bufferMask, contextID);
                _fbo->unBind();
                _renderer->render(_sceneManager->getRootNode(), pass, camera, contextID);
            }
		}
	}
}
