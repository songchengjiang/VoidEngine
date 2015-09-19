#include "RenderQueue.h"
#include "RenderCommand.h"
#include "Renderer.h"
#include "Visualiser.h"
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
	RenderCommandList &list = _renderQueues[cmd.camera];
	list[renderQueueType].push_back(cmd);
}

void veRenderQueue::execute(veVisualiser *vs)
{
	for (auto &iter : _renderQueues) {
		iter.first->render(vs, iter.second);
	}
	_renderQueues.clear();
}
