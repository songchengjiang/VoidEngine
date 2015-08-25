#ifndef _VE_RENDER_COMMAND_
#define _VE_RENDER_COMMAND_
#include "Prerequisites.h"
#include "BaseCore/Matrix4.h"
#include <functional>

class veRenderer;
class veVisualiser;
class veRenderableObject;
class veNode;
class vePass;
struct veRenderCommand
{
	veRenderCommand()
		: priority(NORMAL_PRIORITY){
	}

	enum Priority
	{
		LOW_PRIORITY    = -1,
		NORMAL_PRIORITY = 0,
		HIGH_PRIORITY   = 1,
	};

	int                 priority;
	veMat4              M;
	veMat4              V;
	veMat4              P;
	vePass             *pass;
	veNode             *attachedNode;
	veRenderableObject *renderableObj;
	veVisualiser       *visualizer;
	veRenderer         *renderer;
};

#endif
