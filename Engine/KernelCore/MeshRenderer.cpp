#include "MeshRenderer.h"
#include "RenderCommand.h"
#include "Visualiser.h"
#include "RenderableObject.h"
#include "Mesh.h"
#include "Material.h"

veMeshRenderer::veMeshRenderer()
{
}

veMeshRenderer::~veMeshRenderer()
{

}

void veMeshRenderer::render(veRenderableObject *renderableObj, veVisualiser *vs)
{
	if (renderableObj){
		veRenderer::render(renderableObj, vs);
		if (_technique){
			veMesh *mesh = static_cast<veMesh *>(renderableObj);
			for (unsigned int i = 0; i < _technique->getPassNum(); ++i){
				auto pass = _technique->getPass(i);
			}
		}
	}
}