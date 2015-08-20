#ifndef _VE_MESH_RENDERER_
#define _VE_MESH_RENDERER_
#include "Renderer.h"

class VE_EXPORT veMeshRenderer : public veRenderer
{
public:
	veMeshRenderer();
	~veMeshRenderer();

	virtual void render(veRenderableObject *renderableObj, veVisualiser *vs) override;
};

#endif
