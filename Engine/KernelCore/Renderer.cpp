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
	auto material = renderableObj->getMaterial();
	if (material)
		_technique = findOptimalTechnique(material);
}

veTechnique* veRenderer::findOptimalTechnique(veMaterial *material)
{
	return material->activeTechnique();
}
