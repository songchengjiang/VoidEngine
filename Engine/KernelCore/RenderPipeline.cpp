#include "RenderPipeline.h"
#include "SceneManager.h"
#include "RenderQueue.h"
#include "Camera.h"

static veLoopQueue<veRenderCommand>::SortFunc PASS_SORT = [](const veRenderCommand &left, const veRenderCommand &right)->bool {
	return right.priority == left.priority ? left.pass <= right.pass : right.priority <= left.priority;
};

static veLoopQueue<veRenderCommand>::SortFunc ENTITY_SORT = [](const veRenderCommand &left, const veRenderCommand &right)->bool {
	return right.priority == left.priority ? left.pass <= right.pass : right.priority <= left.priority;
};

static veLoopQueue<veRenderCommand>::SortFunc TRANSPARENT_SORT = [](const veRenderCommand &left, const veRenderCommand &right)->bool {
	return right.priority == left.priority ? left.depthInCamera <= right.depthInCamera : right.priority <= left.priority;
};

static veLoopQueue<veRenderCommand>::SortFunc OVERLAY_SORT = [](const veRenderCommand &left, const veRenderCommand &right)->bool {
	return left.priority <= right.priority;
};

veRenderPipeline::veRenderPipeline(veSceneManager *sm)
	: USE_VE_PTR_INIT
	, _sceneManager(sm)
{

}

veRenderPipeline::~veRenderPipeline()
{

}

void veRenderPipeline::rendering()
{
	fillRenderQueues();
	sortRenderQueues();
	visitRenderQueues();
}

void veRenderPipeline::pushVisibleNode(veCamera *camera, veNode *node)
{
	_visibleNodeList[camera].push_back(node);
}

bool veRenderPipeline::isNodeVisible(veNode *node)
{
	std::unique_lock<std::mutex> lock(_visitMutex);
	for (auto &iter : _visibleNodeList) {
		for (auto &n : iter.second) {
			if (node == n)
				return true;
		}
	}
	return false;
}

void veRenderPipeline::fillRenderQueues()
{
	auto &cameraList = _sceneManager->getCameraList();
	for (auto &iter : cameraList) {
		veCamera *camera = iter.get();
		_sceneManager->enqueueTaskToThread(nullptr, nullptr, [camera, this] {
			std::unique_lock<std::mutex> lock(_visitMutex);
			_visibleNodeList[camera].clear();
			camera->cull(this);
		});

		//if (!camera->getVisibleNodeList().empty()) {
		//	for (auto &iter : camera->getVisibleNodeList()) {
		//		for (unsigned int i = 0; i < iter->getRenderableObjectCount(); ++i) {
		//			if (iter->getRenderableObject(i)->isVisible())
		//				iter->getRenderableObject(i)->render(iter, camera);
		//		}
		//	}
		//}
	}

	{
		std::unique_lock<std::mutex> lock(_visitMutex);
		for (auto &iter : _visibleNodeList) {
			auto &camera = iter.first;
			auto &nodeList = iter.second;
			if (!nodeList.empty()) {
				for (auto &node : nodeList) {
					for (unsigned int i = 0; i < node->getRenderableObjectCount(); ++i) {
						if (node->getRenderableObject(i)->isVisible())
							node->getRenderableObject(i)->render(node, camera);
					}
				}
			}
		}
	}
}

void veRenderPipeline::sortRenderQueues()
{
	auto &cameraList = _sceneManager->getCameraList();
	for (auto &iter : cameraList) {
		veCamera *camera = iter.get();
		if (!camera->getRenderQueue()->renderCommandList.empty()) {
			//_sceneManager->enqueueTaskToThread(nullptr, nullptr, [camera] {
				for (auto &renderPass : camera->getRenderQueue()->renderCommandList) {
					auto &renderList = renderPass.second;
					auto bgQueue = renderList.find(veRenderQueue::RENDER_QUEUE_BACKGROUND);
					if (bgQueue != renderList.end()) {
						if (!bgQueue->second.empty()) {
							bgQueue->second.quickSort(PASS_SORT);
						}
					}

					auto entityQueue = renderList.find(veRenderQueue::RENDER_QUEUE_ENTITY);
					if (entityQueue != renderList.end()) {
						if (!entityQueue->second.empty()) {
							entityQueue->second.quickSort(ENTITY_SORT);
						}
					}

					auto tpQueue = renderList.find(veRenderQueue::RENDER_QUEUE_TRANSPARENT);
					if (tpQueue != renderList.end()) {
						if (!tpQueue->second.empty()) {
							tpQueue->second.quickSort(TRANSPARENT_SORT);
						}
					}

					auto olQueue = renderList.find(veRenderQueue::RENDER_QUEUE_OVERLAY);
					if (olQueue != renderList.end()) {
						if (!olQueue->second.empty()) {
							olQueue->second.quickSort(OVERLAY_SORT);
						}
					}
				}
			//});
		}
	}
}

void veRenderPipeline::visitRenderQueues()
{
	auto &cameraList = _sceneManager->getCameraList();
	for (auto &iter : cameraList) {
		veCamera *camera = iter.get();
		if (!camera->getViewport().isNull()) {
			veRenderState::instance()->resetState();
			renderCamera(camera);
		}
	}
}

void veRenderPipeline::draw(veCamera *camera)
{
	auto &vp = camera->getViewport();
	auto &clearColor = camera->getClearColor();
	glViewport(vp.x * VE_DEVICE_PIXEL_RATIO, vp.y * VE_DEVICE_PIXEL_RATIO, vp.width * VE_DEVICE_PIXEL_RATIO, vp.height * VE_DEVICE_PIXEL_RATIO);
	glClear(camera->getClearMask());
	glClearColor(clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a());

	if (!camera->getRenderQueue()->renderCommandList.empty()) {
		for (auto &renderPass : camera->getRenderQueue()->renderCommandList) {
			auto &renderList = renderPass.second;
			for (auto &rq : renderList) {
				size_t sz = rq.second.size();
				for (size_t i = 0; i < sz; ++i) {
					auto &cmd = rq.second[i];
					cmd.renderer->draw(cmd);
				}
			}
		}
		camera->getRenderQueue()->renderCommandList.clear();
	}
}

void veRenderPipeline::renderCamera(veCamera *camera)
{
	if (camera->getFrameBufferObject()) {
		camera->getFrameBufferObject()->bind(camera->getClearMask());
	}
	draw(camera);
	if (camera->getFrameBufferObject()) {
		camera->getFrameBufferObject()->unBind();
	}
}
