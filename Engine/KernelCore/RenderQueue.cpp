#include "RenderQueue.h"
#include "RenderCommand.h"
#include "Renderer.h"
#include "Camera.h"

veRenderQueue * veRenderQueue::CURRENT_RENDER_QUEUE = nullptr;

veRenderQueue::veRenderQueue()
{

}

veRenderQueue::~veRenderQueue()
{

}

void veRenderQueue::pushCommand(unsigned int renderQueueType, const veRenderCommand &cmd)
{
	_renderQueue[renderQueueType].push_back(cmd);
}

void veRenderQueue::execute(veCamera *camera)
{
	camera->render(_renderQueue);
	_renderQueue.clear();
}
