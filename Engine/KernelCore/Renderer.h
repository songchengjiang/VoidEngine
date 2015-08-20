#ifndef _VE_RENDERER_
#define _VE_RENDERER_
#include "Component.h"
#include "RenderCommand.h"
#include "VE_Ptr.h"

class veRenderableObject;
class VE_EXPORT veRenderer
{
public:
	veRenderer();
	virtual ~veRenderer();

	virtual void render(veRenderableObject *renderableObj, veVisualiser *vs) = 0;
	veRenderCommand* getRenderCommand() { return _renderCommand.get(); }

protected:

	VE_Ptr<veRenderCommand> _renderCommand;
};

#endif
