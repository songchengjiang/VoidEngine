#include "OctreeSceneManager.h"
#include "Octree.h"
#include "OctreeNode.h"
#include "OctreeCamera.h"
#include "OctreeRenderQueue.h"

veOctreeSceneManager::veOctreeSceneManager()
	: _boundingBox(veVec3(-10000.0f), veVec3(10000.0f))
	, _octreeMaxDeep(8)
	, _octree(new veOctree)
{
	init();
}

veOctreeSceneManager::veOctreeSceneManager(const veBoundingBox &bbox, unsigned int octreeDeep)
	: _boundingBox(bbox)
	, _octreeMaxDeep(octreeDeep)
	, _octree(new veOctree)
{
	init();
}

veOctreeSceneManager::~veOctreeSceneManager()
{
}

void veOctreeSceneManager::init()
{
	_root = new veOctreeNode;
	_renderQueue = new veOctreeRenderQueue;
	_octree->boundingBox = _boundingBox;
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

void veOctreeSceneManager::updateRenderableNode(veNode *node)
{

}

void veOctreeSceneManager::update()
{
	_root->update(this, veMat4::IDENTITY);
}

void veOctreeSceneManager::render()
{
	//glClear(_clearMask);
	veRenderQueue::CURRENT_RENDER_QUEUE = _renderQueue;
	auto mainCamera = _visualiser->getCamera();
	for (auto &iter : _cameraList) {
		if (iter->getParent() && iter != mainCamera) {
			if (iter->getFrameBufferObject()) {
				_root->render(iter);
			}
		}
	}
	if (mainCamera)
		_root->render(mainCamera);
	_renderQueue->execute(_visualiser.get());
	veSceneManager::render();
}

void veOctreeSceneManager::updateOctree()
{

}