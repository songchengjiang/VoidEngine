#ifndef _VE_RENDER_COMMAND_
#define _VE_RENDER_COMMAND_
#include "Prerequisites.h"
#include <functional>

class veRenderer;
class veVisualiser;
struct veRenderCommand
{
	veRenderCommand()
		: priority(NORMAL_PRIORITY)
		, delayFrame(0)
		, keepFrame(1)
		, iskeeping(false){
	}

	enum Priority
	{
		LOW_PRIORITY,
		NORMAL_PRIORITY,
		HIGH_PRIORITY,
	};

	Priority priority;
	unsigned int delayFrame;
	unsigned int keepFrame;
	bool iskeeping;
	std::function<void(double /*deltaTime*/, veVisualiser*)> renderFunc;
};

#endif
