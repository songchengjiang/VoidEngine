#include "RenderableObject.h"
#include "Visualiser.h"

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
		vs->getRenderQueue().pushCommand(veRenderCommand::NORMAL_PRIORITY, _renderer->getRenderCommand());
	}
}

void veRenderableObject::setRenderer(veRenderer *renderer)
{
	_renderer = renderer;
}
