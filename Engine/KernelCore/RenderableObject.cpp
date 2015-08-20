#include "RenderableObject.h"
#include "Visualiser.h"

veRenderableObject::veRenderableObject()
	: _renderer(nullptr)
{
	USE_VE_PTR_INIT
}

veRenderableObject::~veRenderableObject()
{

}

void veRenderableObject::update(veNode *node, veVisualiser *vs)
{
	updateImp(node, vs);
	if (_renderer.valid()){
		_renderer->getRenderCommand()->renderableObj = this;
		vs->getRenderQueue().pushCommand(veRenderCommand::NORMAL_PRIORITY, _renderer->getRenderCommand());
	}
}

void veRenderableObject::setRenderer(veRenderer *renderer)
{
	_renderer = renderer;
}
