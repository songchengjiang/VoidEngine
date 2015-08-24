#include "Renderer.h"
#include "RenderableObject.h"

veRenderer::veRenderer()
	: USE_VE_PTR_INIT
	, _technique(nullptr)
{
}

veRenderer::~veRenderer()
{
}

void veRenderer::visit(veNode *node, veRenderableObject *renderableObj, veVisualiser *vs)
{
	if (!_technique){
		auto material = renderableObj->getMaterial();
		_technique = findOptimalTechnique(material);
	}
}

veTechnique* veRenderer::findOptimalTechnique(veMaterial *material)
{
	return material->getTechnique(0);
}
