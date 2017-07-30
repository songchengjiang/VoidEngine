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
    , _isLocalBoundingBox(true)
{
}

veRenderableObject::~veRenderableObject()
{

}

void veRenderableObject::update(veNode *node, veSceneManager *sm)
{
	if (!_isVisible) return;
}

void veRenderableObject::render(veNode *node, veCamera *camera, unsigned int contextID)
{
	if (!_isVisible) return;
	if (_renderer.valid() && _material.valid())
		_renderer->render(this, camera, node->getNodeToWorldMatrix(), contextID);
}
