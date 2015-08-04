#ifndef _VE_RENDER_COMMAND_
#define _VE_RENDER_COMMAND_
#include "Prerequisites.h"
#include <functional>

class veRenderer;
struct RenderCommand
{
	RenderCommand()
		: priority(NORMAL_PRIORITY)
		, updateFrequency(0)
		, keepFrame(0){
	}

	enum Priority
	{
		LOW_PRIORITY,
		NORMAL_PRIORITY,
		HIGH_PRIORITY,
	};

	Priority priority;
	unsigned int updateFrequency;
	unsigned int keepFrame;
	std::function<void(veRenderer *renderer)> renderFunc;
};

#endif
