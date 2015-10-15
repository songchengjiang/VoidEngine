#include "OctreeSceneManager.h"
#include "OctreeNode.h"
#include "OctreeCamera.h"

veOctreeSceneManager::veOctreeSceneManager()
	: _boundingBox(veVec3(-10000.0f), veVec3(10000.0f))
	, _octreeMaxDeep(8)
	, _octree(nullptr)
{
	_root = new veOctreeNode;
}

veOctreeSceneManager::veOctreeSceneManager(const veBoundingBox &bbox, unsigned int octreeDeep)
	: _boundingBox(bbox)
	, _octreeMaxDeep(octreeDeep)
{

}

veOctreeSceneManager::~veOctreeSceneManager()
{

}

veNode* veOctreeSceneManager::createNode()
{
	auto node = new veOctreeNode;
	node->setSceneManager(this);
	_nodeList.push_back(node);
	return node;
}

veCamera* veOctreeSceneManager::createCamera(const veViewport &vp)
{
	auto camera = new veOctreeCamera(vp);
	camera->setSceneManager(this);
	_cameraList.push_back(camera);
	return camera;
}

void veOctreeSceneManager::initOctrees()
{

}

void veOctreeSceneManager::update()
{
	_root->update(this);
}

void veOctreeSceneManager::cull()
{

}

void veOctreeSceneManager::render()
{
	//glClear(_clearMask);
	veRenderQueue::CURRENT_RENDER_QUEUE = &_renderQueue;
	auto mainCamera = _visualiser->getCamera();
	for (auto &iter : _cameraList) {
		if (iter->getParent() && iter != mainCamera) {
			if (iter->getFrameBufferObject())
				_root->render(iter);
		}
	}
	if (mainCamera)
		_root->render(mainCamera);
	_renderQueue.execute(_visualiser.get());
	veSceneManager::render();
}
