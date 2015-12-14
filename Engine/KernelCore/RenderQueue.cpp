#include "RenderQueue.h"
#include "RenderCommand.h"
#include "Renderer.h"
#include "Camera.h"

veRenderQueue::veRenderQueue()
{

}

veRenderQueue::~veRenderQueue()
{

}

void veRenderQueue::pushCommand(unsigned int renderPassIndex, unsigned int renderQueueType, const veRenderCommand &cmd)
{
	auto &passRenderQueue = renderCommandList[renderPassIndex];
	passRenderQueue[renderQueueType].push_back(cmd);
}