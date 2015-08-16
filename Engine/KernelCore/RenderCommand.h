#ifndef _VE_RENDER_COMMAND_
#define _VE_RENDER_COMMAND_
#include "Prerequisites.h"
#include <functional>

class veRenderer;
class veVisualiser;
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

	int priority;
	std::function<void(double /*deltaTime*/, veVisualiser*)> renderFunc;
};

#endif
