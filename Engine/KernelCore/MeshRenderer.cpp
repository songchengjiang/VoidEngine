#include "MeshRenderer.h"
#include "RenderCommand.h"
#include "Visualiser.h"
#include "RenderableObject.h"
#include "Mesh.h"

veMeshRenderer::veMeshRenderer()
{
}

veMeshRenderer::~veMeshRenderer()
{

}

void veMeshRenderer::render(veRenderableObject *renderableObj, veVisualiser *vs)
{
	if (renderableObj){
		veMesh *mesh = static_cast<veMesh *>(renderableObj);

	}
}
