#include "PostProcesser.h"
#include "SceneManager.h"
#include "FrameBufferObject.h"

vePostProcesser::vePostProcesser(veSceneManager *sm)
	: USE_VE_PTR_INIT
	, _sceneManager(sm)
{
}

vePostProcesser::~vePostProcesser()
{

}

void vePostProcesser::process(veCamera *camera)
{
	if (!_surface.valid()) {
		_surface = _sceneManager->createSurface(_name + std::string("-surface"));
		_surface->setMaterialArray(_materials.get());
	}
	_surface->update(_sceneManager->getRootNode(), _sceneManager);
	_surface->render(_sceneManager->getRootNode(), camera);

}

void vePostProcesser::attachFrameBuffer(veFrameBufferObject *fb)
{
	fb->attach(GL_COLOR_ATTACHMENT0, _materials->getMaterial(0)->getTechnique(0)->getPass(0)->getTexture(0));
}
