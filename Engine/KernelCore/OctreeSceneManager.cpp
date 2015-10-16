#include "OctreeSceneManager.h"
#include "Octree.h"
#include "OctreeNode.h"
#include "OctreeCamera.h"
#include "OctreeRenderQueue.h"
#include "RenderableObject.h"

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
	_root->setSceneManager(this);
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

void veOctreeSceneManager::requestRender(veNode *node)
{
	veOctreeNode *ocNode = static_cast<veOctreeNode *>(node);
	if (!ocNode) return;
	if (!ocNode->octant) {
		if (!ocNode->isIn(_octree->boundingBox)) {
			_octree->addNode(ocNode);
		}
		else {
			addOctreeNode(ocNode, _octree);
		}
	}
	else {
		if (!ocNode->isIn(ocNode->octant->boundingBox)) {
			ocNode->octant->removeNode(ocNode);
			
			if (!ocNode->isIn(_octree->boundingBox)) {
				_octree->addNode(ocNode);
			}
			else {
				addOctreeNode(ocNode, _octree);
			}
		}
	}
}

void veOctreeSceneManager::addOctreeNode(veOctreeNode *node, veOctree *octant, unsigned int depth)
{
	if (!octant) return;

	const veBoundingBox &bbox = node->getBoundingBox();
	if (depth < _octreeMaxDeep && octant->isTwiceSize(bbox)) {
		unsigned int x, y, z;
		octant->getChildIndex(bbox, x, y, z);
		if (!octant->getChild(x, y, z)) {
			auto octree = new veOctree;
			const veVec3& octantMin = octant->boundingBox.min();
			const veVec3& octantMax = octant->boundingBox.max();
			veVec3 &min = octree->boundingBox.min();
			veVec3 &max = octree->boundingBox.max();
			if (x == 0)
			{
				min.x() = octantMin.x();
				max.x() = (octantMin.x() + octantMax.x()) * 0.5f;
			}
			else
			{
				min.x() = (octantMin.x() + octantMax.x()) * 0.5f;
				max.x() = octantMax.x();
			}
			if (y == 0)
			{
				min.y() = octantMin.y();
				max.y() = (octantMin.y() + octantMax.y()) * 0.5f;
			}
			else
			{
				min.y() = (octantMin.y() + octantMax.y()) * 0.5f;
				max.y() = octantMax.y();
			}
			if (z == 0)
			{
				min.z() = octantMin.z();
				max.z() = (octantMin.z() + octantMax.z()) * 0.5f;
			}
			else
			{
				min.z() = (octantMin.z() + octantMax.z()) * 0.5f;
				max.z() = octantMax.z();
			}
			octant->setChild(x, y, z, octree);	
		}

		addOctreeNode(node, octant->getChild(x, y, z), ++depth);
	}
	else {
		octant->addNode(node);
	}
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
		if (iter->isVisible() && iter->isInScene() && iter != mainCamera) {
			if (iter->getFrameBufferObject()) {
				//_root->render(iter);
				traverseOctree(_octree, iter);
			}
		}
	}

	if (mainCamera && mainCamera->isInScene() && mainCamera->isVisible()) {
		//_root->render(mainCamera);
		traverseOctree(_octree, mainCamera);
	}
	_renderQueue->execute(_visualiser.get());
	veSceneManager::render();
}

void veOctreeSceneManager::traverseOctree(veOctree *octant, veCamera *camera)
{
	if (camera->isOutOfFrustum(octant->boundingBox))
		return;

	if (!octant->nodeList.empty()) {
		for (auto &iter : octant->nodeList) {
			if (iter->isVisible() && (iter->getMask() & camera->getMask())) {
				for (unsigned int i = 0; i < iter->getRenderableObjectCount(); ++i) {
					iter->getRenderableObject(i)->render(iter, camera);
				}
			}
		}
	}

	for (unsigned int i = 0; i < 8; ++i) {
		if (octant->children[i]) {
			traverseOctree(octant->children[i], camera);
		}
	}
}
