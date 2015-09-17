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
	sortQueues();
	for (auto &iter : _renderQueues) {
		iter.first->render(vs, iter.second);
	}
	_renderQueues.clear();
}

void veRenderQueue::sortQueues()
{
	auto sortFunc = [](const veRenderCommand &left, const veRenderCommand &right)->bool {
		return right.priority <= left.priority && left.pass <= right.pass;
	};

	auto transparentSortFunc = [](const veRenderCommand &left, const veRenderCommand &right)->bool {
		return /*right.priority <= left.priority && */left.depthInCamera <= right.depthInCamera && left.pass <= right.pass;
	};

	for (auto &iter : _renderQueues) {
		auto bgQueue = iter.second.find(veRenderQueue::RENDER_QUEUE_BACKGROUND);
		if (bgQueue != iter.second.end() && !bgQueue->second.empty())
			bgQueue->second.sort(sortFunc);

		auto entityQueue = iter.second.find(veRenderQueue::RENDER_QUEUE_ENTITY);
		if (entityQueue != iter.second.end() && !entityQueue->second.empty())
			entityQueue->second.sort(sortFunc);

		auto tpQueue = iter.second.find(veRenderQueue::RENDER_QUEUE_TRANSPARENT);
		if (tpQueue != iter.second.end() && !tpQueue->second.empty())
			tpQueue->second.sort(transparentSortFunc);

		auto olQueue = iter.second.find(veRenderQueue::RENDER_QUEUE_OVERLAY);
		if (olQueue != iter.second.end() && !olQueue->second.empty())
			olQueue->second.sort(sortFunc);
	}
}
