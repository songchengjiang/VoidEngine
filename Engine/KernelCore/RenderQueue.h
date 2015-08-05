#ifndef _VE_RENDER_QUEUE_
#define _VE_RENDER_QUEUE_
#include "Prerequisites.h"
#include "LoopQueue.h"
#include "VE_Ptr.h"
#include "RenderCommand.h"

class veVisualiser;
class veRenderQueue
{
public:
	veRenderQueue();
	~veRenderQueue();

	void pushCommand(veRenderCommand *cmd);
	void execute(double deltaTime, veVisualiser *visualiser);

private:

	void executeCmd(LoopQueue< VE_Ptr<veRenderCommand> > &queue, veRenderCommand *cmd, double deltaTime, veVisualiser *visualiser);

private:

	LoopQueue< VE_Ptr<veRenderCommand> > _highPriorityQueue;
	LoopQueue< VE_Ptr<veRenderCommand> > _normalPriorityQueue;
	LoopQueue< VE_Ptr<veRenderCommand> > _lowPriorityQueue;
};

#endif