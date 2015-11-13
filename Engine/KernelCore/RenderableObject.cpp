#include "RenderableObject.h"
#include "Camera.h"
#include "RenderQueue.h"

veRenderableObject::veRenderableObject()
	: USE_VE_PTR_INIT
	, _renderer(nullptr)
	, _isVisible(true)
	, _isDirtyBoundingBox(true)
	, _mask(0xffffffff)
	, _sceneManager(nullptr)
{
}

veRenderableObject::~veRenderableObject()
{

}

void veRenderableObject::update(veNode *node, veSceneManager *sm)
{
	if (!_isVisible) return;
	if (_renderer.valid())
		_renderer->visit(node, this, sm);
}

void veRenderableObject::render(veNode *node, veCamera *camera)
{
	if (!_isVisible) return;
	if (camera->getMask() & _mask) {
		if (_renderer.valid())
			_renderer->render(node, this, camera);
	}
}