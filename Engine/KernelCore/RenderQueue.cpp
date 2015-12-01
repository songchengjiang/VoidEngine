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
	auto &passRenderQueue = _renderQueue[renderPassIndex];
	passRenderQueue[renderQueueType].push_back(cmd);
}

void veRenderQueue::execute(veCamera *camera)
{
	for (auto &renderPass : _renderQueue) {
		camera->render(renderPass.second);
	}
	_renderQueue.clear();
}
