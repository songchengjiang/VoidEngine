#include "Renderer.h"
#include "RenderableObject.h"

veRenderer::veRenderer()
	: _renderCommand(new veRenderCommand)
{
	USE_VE_PTR_INIT
	_renderCommand->renderer = this;
}

veRenderer::~veRenderer()
{
	VE_SAFE_DELETE(_renderCommand);
}
