#include "PostProcesser.h"
#include "SceneManager.h"
#include "FrameBufferObject.h"

vePostProcesser::vePostProcesser(veSceneManager *sm)
	: USE_VE_PTR_INIT
	, _sceneManager(sm)
{
	_render = new vePostProcesserRenderer;
}

vePostProcesser::~vePostProcesser()
{

}

void vePostProcesser::process(veFrameBufferObject *fb, veCamera *camera)
{
	if (!_surface.valid()) {
		_surface = _sceneManager->createSurface(_name + std::string("-surface"));
		_surface->setRenderer(_render.get());
		_surface->setMaterialArray(_materials.get());
	}
	for (unsigned int i = 0; i < _materials->getMaterialNum(); ++i) {
		auto material = _materials->getMaterial(i);
		for (unsigned int p = 0; p < material->activeTechnique()->getPassNum(); ++p) {
			auto pass = material->activeTechnique()->getPass(p);
			fb->attach(GL_COLOR_ATTACHMENT0, pass->getTexture(0));
			camera->render();
			_render->setPostProcessingPass(pass);
			_surface->render(_sceneManager->getRootNode(), camera);
		}
	}
}
