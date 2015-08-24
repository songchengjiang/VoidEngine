#ifndef _VE_RENDERER_
#define _VE_RENDERER_
#include "Component.h"
#include "RenderCommand.h"
#include "VE_Ptr.h"

class veRenderableObject;
class veTechnique;
class veMaterial;
class vePass;
class VE_EXPORT veRenderer
{
	USE_VE_PTR
public:
	veRenderer();
	virtual ~veRenderer();
	virtual void visit(veNode *node, veRenderableObject *renderableObj, veVisualiser *vs) = 0;
	virtual void render(vePass *pass) = 0;

protected:

	virtual veTechnique* findOptimalTechnique(veMaterial *material);

protected:

	veTechnique     *_technique;
};

#endif
