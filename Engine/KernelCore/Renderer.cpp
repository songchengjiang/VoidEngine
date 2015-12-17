#include "Renderer.h"
#include "RenderableObject.h"
#include "Shader.h"
#include "Constants.h"
#include "Node.h"
#include "Camera.h"

unsigned short veRenderer::CURRENT_RENDER_STAGE = RENDERING;

veRenderer::veRenderer()
	: USE_VE_PTR_INIT
	, _renderStageMask(LIGHTINGING | RENDERING)
{
}

veRenderer::~veRenderer()
{
}

void veRenderer::visit(veNode *node, veRenderableObject *renderableObj, veSceneManager *sm)
{
}

bool veRenderer::isNeedRendering()
{
	return (CURRENT_RENDER_STAGE & _renderStageMask) != 0;
}
