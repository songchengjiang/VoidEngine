#include "Renderer.h"
#include "RenderableObject.h"

veRenderer::veRenderer()
	: _renderCommand(new veRenderCommand)
{
	_renderCommand->renderer = this;
}

veRenderer::~veRenderer()
{

}
