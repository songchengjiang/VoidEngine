#ifndef _VE_MESH_RENDERER_
#define _VE_MESH_RENDERER_
#include "Renderer.h"
#include "VE_Ptr.h"

struct veRenderCommand;
class VE_EXPORT veMeshRenderer : public veRenderer
{
public:
	veMeshRenderer();
	~veMeshRenderer();

	virtual void update(double deltaTime, veNode *node, veVisualiser *vs) override;

private:

	void render(double deltaTime, veVisualiser *vs);

private:

	VE_Ptr<veRenderCommand> _renderCommand;
	veNode *_node;
};

#endif
