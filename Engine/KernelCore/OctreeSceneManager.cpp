#include "OctreeSceneManager.h"
#include "OctreeNode.h"
#include "OctreeCamera.h"

veOctreeSceneManager::veOctreeSceneManager()
{

}

veOctreeSceneManager::~veOctreeSceneManager()
{

}

veNode* veOctreeSceneManager::createNode()
{
	auto node = new veOctreeNode;
	node->setSceneManager(this);
	return node;
}

veCamera* veOctreeSceneManager::createCamera(const veViewport &vp)
{
	auto camera = new veOctreeCamera(vp);
	camera->setSceneManager(this);
	return camera;
}

veLight* veOctreeSceneManager::createLight(const std::string &className)
{
	auto light = veLightManager::instance()->instanceOneLight(className);
	light->setSceneManager(this);
	return light;
}

void veOctreeSceneManager::update()
{
	if (!_root.get()) return;
	_root->update(this);
	//if (!_visualiser->getCamera()->getParent() && _root.get() != _visualiser->getCamera())
	//	_visualiser->getCamera()->update(_visualiser.get());
}

void veOctreeSceneManager::cull()
{

}

void veOctreeSceneManager::render()
{
	if (!_root.get()) return;
	//glClear(_clearMask);
	veRenderQueue::CURRENT_RENDER_QUEUE = &_renderQueue;
	veCamera *mainCamera = nullptr;
	for (auto &iter : _cameraList) {
		if (iter == _visualiser->getCamera()) 
			mainCamera = iter;
		if (iter->getFrameBufferObject())
			_root->render(iter);
	}
	if (mainCamera)
		_root->render(mainCamera);
	_renderQueue.execute(_visualiser.get());
	veSceneManager::render();
}
