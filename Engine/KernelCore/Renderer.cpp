#include "Renderer.h"
#include "RenderableObject.h"

veRenderer::veRenderer()
	: USE_VE_PTR_INIT
	, _renderCommand(new veRenderCommand)
	, _technique(nullptr)
{
	_renderCommand->renderer = this;
}

veRenderer::~veRenderer()
{
	VE_SAFE_DELETE(_renderCommand);
}

void veRenderer::render()
{
	if (!_technique){
		auto material = _renderCommand->renderableObj->getMaterial();
		_technique = findOptimalTechnique(material);
	}
}

veTechnique* veRenderer::findOptimalTechnique(veMaterial *material)
{
	return material->getTechnique(0);
}
