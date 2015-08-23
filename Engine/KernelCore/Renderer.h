#ifndef _VE_RENDERER_
#define _VE_RENDERER_
#include "Component.h"
#include "RenderCommand.h"
#include "VE_Ptr.h"

class veRenderableObject;
class veTechnique;
class veMaterial;
class VE_EXPORT veRenderer
{
	USE_VE_PTR
public:
	veRenderer();
	virtual ~veRenderer();

	virtual void render() = 0;
	veRenderCommand* getRenderCommand() { return _renderCommand; }

protected:

	virtual veTechnique* findOptimalTechnique(veMaterial *material);

protected:

	veRenderCommand *_renderCommand;
	veTechnique     *_technique;
};

#endif
