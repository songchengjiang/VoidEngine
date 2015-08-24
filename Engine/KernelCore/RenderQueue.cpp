#include "RenderQueue.h"
#include "RenderCommand.h"
#include "Renderer.h"

veRenderQueue::veRenderQueue()
{

}

veRenderQueue::~veRenderQueue()
{

}

void veRenderQueue::pushCommand(unsigned int renderQueueType, veRenderCommand *cmd)
{
	_renderQueues[renderQueueType].push_back(cmd);
}

void veRenderQueue::execute(veVisualiser *visualiser)
{
	for (auto &iter : _renderQueues){
		auto &q = iter.second;
		q.sort([](const veRenderCommand *left, const veRenderCommand *right)->bool{
			return right->priority < left->priority;
		});
		while (!q.empty()){
			const auto &cmd = q.front();
			cmd->renderer->render(cmd->pass);
			q.pop_front();
		}
	}
}
