#ifndef _VE_RENDER_QUEUE_
#define _VE_RENDER_QUEUE_
#include "Prerequisites.h"
#include "RenderCommand.h"
#include "LoopQueue.h"
#include <map>
#include <unordered_map>

class veVisualiser;
class veRenderQueue
{
public:
	static veRenderQueue *CURRENT_RENDER_QUEUE;
	typedef std::map<unsigned int, veLoopQueue< veRenderCommand > > RenderCommandList;

	enum RenderQueueType
	{
		RENDER_QUEUE_BACKGROUND  = 0,
		RENDER_QUEUE_ENTITY      = 50,
		RENDER_QUEUE_TRANSPARENT = 100,
		RENDER_QUEUE_OVERLAY     = 150,
	};

	veRenderQueue();
	~veRenderQueue();

	void pushCommand(unsigned int renderQueueType, const veRenderCommand &cmd);
	void execute();

private:

	void sortQueues();

private:

	std::unordered_map<veCamera*, RenderCommandList> _renderQueues;
};

#endif