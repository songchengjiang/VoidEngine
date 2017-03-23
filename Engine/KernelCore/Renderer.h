#ifndef _VE_RENDERER_
#define _VE_RENDERER_
#include "Component.h"
#include "RenderCommand.h"
#include "GLDataBuffer.h"
#include "VE_Ptr.h"

class veRenderableObject;
class veTechnique;
class veMaterial;
class vePass;
class veUniform;
class VE_EXPORT veRenderer
{
public:

	veRenderer();
	USE_VE_PTR;

	virtual ~veRenderer();
	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID) = 0;
	virtual void draw(veRenderCommand &command) = 0;

};

#endif
