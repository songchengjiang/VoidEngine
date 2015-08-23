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
	if (_renderer.valid()){
		_renderer->getRenderCommand()->renderableObj = this;
		_renderer->getRenderCommand()->attachedNode = node;
		_renderer->getRenderCommand()->visualizer = vs;
		vs->getRenderQueue().pushCommand(veRenderQueue::RENDER_QUEUE_ENTITY, _renderer->getRenderCommand());
	}
}

void veRenderableObject::setRenderer(veRenderer *renderer)
{
	_renderer = renderer;
}
