#include "RenderQueue.h"
#include "RenderCommand.h"

veRenderQueue::veRenderQueue()
{

}

veRenderQueue::~veRenderQueue()
{

}

void veRenderQueue::pushCommand(veRenderCommand *cmd)
{
	if (cmd->priority == veRenderCommand::NORMAL_PRIORITY){
		_normalPriorityQueue.push_back(cmd);
	}
	else if (cmd->priority == veRenderCommand::HIGH_PRIORITY){
		_highPriorityQueue.push_back(cmd);
	}
	else{
		_lowPriorityQueue.push_back(cmd);
	}
}

void veRenderQueue::execute(double deltaTime, veVisualiser *visualiser)
{
	while (!_highPriorityQueue.empty()){
		auto cmd = _highPriorityQueue.front();
		_highPriorityQueue.pop_front();
		executeCmd(_highPriorityQueue, cmd.get(), deltaTime, visualiser);
	}

	while (!_normalPriorityQueue.empty()){
		auto cmd = _normalPriorityQueue.front();
		_normalPriorityQueue.pop_front();
		executeCmd(_normalPriorityQueue, cmd.get(), deltaTime, visualiser);
	}

	while (!_lowPriorityQueue.empty()){
		auto cmd = _lowPriorityQueue.front();
		_lowPriorityQueue.pop_front();
		executeCmd(_lowPriorityQueue, cmd.get(), deltaTime, visualiser);
	}
}

void veRenderQueue::executeCmd(LoopQueue< VE_Ptr<veRenderCommand> > &queue, veRenderCommand *cmd, double deltaTime, veVisualiser *visualiser)
{
	if (0 < cmd->delayFrame) {
		--cmd->delayFrame;
		queue.push_back(cmd);
		return;
	}

	cmd->renderFunc(deltaTime, visualiser);
	if (cmd->iskeeping){
		queue.push_back(cmd);
	}
	else if (1 < cmd->keepFrame){
		--cmd->keepFrame;
		queue.push_back(cmd);
	}
}
