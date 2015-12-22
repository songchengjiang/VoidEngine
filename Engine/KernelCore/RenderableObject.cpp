#include "RenderableObject.h"
#include "Camera.h"
#include "RenderQueue.h"
#include "SceneManager.h"

veRenderableObject::veRenderableObject(veSceneManager *sm)
	: USE_VE_PTR_INIT
	, _sceneManager(sm)
	, _renderer(nullptr)
	, _isVisible(true)
	, _isDirtyBoundingBox(true)
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
	if (_renderer.valid())
		_renderer->render(node, this, camera);
}