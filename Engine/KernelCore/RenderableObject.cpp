#include "RenderableObject.h"
#include "Visualiser.h"
#include "RenderQueue.h"

veRenderableObject::veRenderableObject()
	: USE_VE_PTR_INIT
	, _renderer(nullptr)
{
}

veRenderableObject::~veRenderableObject()
{

}

void veRenderableObject::update(veNode *node, veVisualiser *vs)
{
	if (_renderer.valid())
		_renderer->visit(node, this, vs);
}

void veRenderableObject::render(veNode *node, veCamera *camera)
{
	if (_renderer.valid())
		_renderer->render(node, this, camera);
}

void veRenderableObject::setRenderer(veRenderer *renderer)
{
	_renderer = renderer;
}
