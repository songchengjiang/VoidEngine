#include "PostProcesser.h"
#include "SceneManager.h"

vePostProcesser::vePostProcesser(veSceneManager *sm)
	: USE_VE_PTR_INIT
	, _sceneManager(sm)
{
}

vePostProcesser::~vePostProcesser()
{

}

void vePostProcesser::process(veNode *node, veCamera *camera)
{
	if (!_surface.valid()) {
		_surface = _sceneManager->createSurface(_name + std::string("-surface"));
		_surface->setMaterialArray(_materials.get());
	}

	_surface->update(node, _sceneManager);
	_surface->render(node, camera);
}
