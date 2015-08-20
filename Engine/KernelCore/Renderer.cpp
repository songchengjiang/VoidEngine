#include "Renderer.h"
#include "RenderableObject.h"

veRenderer::veRenderer()
	: USE_VE_PTR_INIT
	, _renderCommand(new veRenderCommand)
{
	_renderCommand->renderer = this;
}

veRenderer::~veRenderer()
{
	VE_SAFE_DELETE(_renderCommand);
}
