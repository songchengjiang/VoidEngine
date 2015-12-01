#ifndef _VE_RENDER_QUEUE_
#define _VE_RENDER_QUEUE_
#include "Prerequisites.h"
#include "RenderCommand.h"
#include "LoopQueue.h"
#include <map>
#include <unordered_map>

class veCamera;
class veRenderQueue
{
public:
	typedef std::map<unsigned int, veLoopQueue< veRenderCommand > > RenderCommandList;

	enum RenderQueueType
	{
		RENDER_QUEUE_BACKGROUND  = 0,
		RENDER_QUEUE_ENTITY      = 50,
		RENDER_QUEUE_TRANSPARENT = 100,
		RENDER_QUEUE_OVERLAY     = 150,
	};

	veRenderQueue();
	virtual ~veRenderQueue();

	virtual void pushCommand(unsigned int renderPassIndex, unsigned int renderQueueType, const veRenderCommand &cmd);
	virtual void execute(veCamera *camera);

protected:

	std::map<unsigned int, RenderCommandList> _renderQueue;
};

#endif