#ifndef _VE_MESH_RENDERER_
#define _VE_MESH_RENDERER_
#include "Renderer.h"

class VE_EXPORT veMeshRenderer : public veRenderer
{
public:
	veMeshRenderer();
	~veMeshRenderer();

	virtual void update(double deltaTime, veNode *node, const veVisualiser *vs) override;
};

#endif
