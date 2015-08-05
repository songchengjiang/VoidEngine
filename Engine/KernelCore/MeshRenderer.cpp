#include "MeshRenderer.h"
#include "RenderCommand.h"
#include "Visualiser.h"

veMeshRenderer::veMeshRenderer()
	: _renderCommand(new veRenderCommand)
{
	_renderCommand->renderFunc = VE_CALLBACK_2(veMeshRenderer::render, this);
}

veMeshRenderer::~veMeshRenderer()
{

}

void veMeshRenderer::update(double deltaTime, veNode *node, veVisualiser *vs)
{
	_node = node;
	vs->getRenderQueue().pushCommand(_renderCommand.get());
}

void veMeshRenderer::render(double deltaTime, veVisualiser *vs)
{
}
