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
	updateImp(node, vs);
	_renderer->visit(node, this, vs);
}

void veRenderableObject::setRenderer(veRenderer *renderer)
{
	_renderer = renderer;
}
