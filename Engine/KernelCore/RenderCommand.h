#ifndef _VE_RENDER_COMMAND_
#define _VE_RENDER_COMMAND_
#include "Prerequisites.h"
#include "BaseCore/Matrix4.h"
#include <functional>

class veRenderer;
class veRenderableObject;
class veCamera;
class veNode;
class vePass;
struct veRenderCommand
{
	veRenderCommand()
		: priority(NORMAL_PRIORITY)
		, depthInCamera(0.0f){
	}

	enum Priority
	{
		LOW_PRIORITY    = -1,
		NORMAL_PRIORITY = 0,
		HIGH_PRIORITY   = 1,
	};

	int                 priority;
	veReal              depthInCamera;
	vePass             *pass;
	veNode             *attachedNode;
	veRenderableObject *renderableObj;
	veCamera           *camera;
	veRenderer         *renderer;
};

#endif
