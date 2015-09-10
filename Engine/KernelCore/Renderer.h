#ifndef _VE_RENDERER_
#define _VE_RENDERER_
#include "Component.h"
#include "RenderCommand.h"
#include "VE_Ptr.h"

class veRenderableObject;
class veTechnique;
class veMaterial;
class vePass;
class veUniform;
class VE_EXPORT veRenderer
{
	USE_VE_PTR
public:
	veRenderer();
	virtual ~veRenderer();
	virtual void visit(veNode *node, veRenderableObject *renderableObj, veVisualiser *vs);
	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) = 0;
	virtual void draw(const veRenderCommand &command) = 0;
	virtual void uniformUpdate(veUniform *uniform, const veRenderCommand &command);

protected:

	virtual veTechnique* findOptimalTechnique(veMaterial *material);

protected:

	veTechnique     *_technique;
};

#endif
