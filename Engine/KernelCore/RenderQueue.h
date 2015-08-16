#ifndef _VE_RENDER_QUEUE_
#define _VE_RENDER_QUEUE_
#include "Prerequisites.h"
#include "VE_Ptr.h"
#include "RenderCommand.h"
#include "LoopQueue.h"
#include <map>

class veVisualiser;
class veRenderQueue
{
public:

	enum RenderQueueType
	{
		RENDER_QUEUE_BACKGROUND  = 0,
		RENDER_QUEUE_ENTITY      = 50,
		RENDER_QUEUE_TRANSPARENT = 100,
		RENDER_QUEUE_OVERLAY     = 150,
	};

	veRenderQueue();
	~veRenderQueue();

	void pushCommand(unsigned int renderQueueType, veRenderCommand *cmd);
	void execute(double deltaTime, veVisualiser *visualiser);

private:

	std::map<unsigned int, veLoopQueue< VE_Ptr<veRenderCommand> > > _renderQueues;
};

#endif