#include "RenderQueue.h"
#include "RenderCommand.h"

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

void veRenderQueue::execute(double deltaTime, veVisualiser *visualiser)
{
	for (auto &iter : _renderQueues){
		auto &q = iter.second;
		q.sort([](const VE_Ptr<veRenderCommand> &left, const VE_Ptr<veRenderCommand> &right)->bool{
			return right->priority < left->priority;
		});
		while (!q.empty()){
			const auto &cmd = q.front();
			cmd->renderFunc(deltaTime, visualiser);
			q.pop_front();
		}
	}
}
