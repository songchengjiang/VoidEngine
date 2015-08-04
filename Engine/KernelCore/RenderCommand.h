#ifndef _VE_RENDER_COMMAND_
#define _VE_RENDER_COMMAND_
#include "Prerequisites.h"
#include <functional>

class veRenderer;
struct veRenderCommand
{
	veRenderCommand()
		: priority(NORMAL_PRIORITY)
		, delayFrame(0)
		, keepFrame(0)
		, isKeeping(false){
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
	bool isKeeping;
	std::function<void(double)> renderFunc;
};

#endif
