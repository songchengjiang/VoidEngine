#include "OctreeSceneManager.h"
#include "Octree.h"
#include "OctreeNode.h"
#include "OctreeCamera.h"
#include "OctreeRenderQueue.h"
#include "RenderableObject.h"
#include "Ray.h"
#include "FileCore/File.h"

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
	_octree->originBoundingBox = _octree->boundingBox = _boundingBox;
}

veNode* veOctreeSceneManager::createNode(const std::string &name)
{
	auto node = new veOctreeNode;
	node->setName(name);
	node->setSceneManager(this);
	_nodeList.push_back(node);
	return node;
}

veCamera* veOctreeSceneManager::createCamera(const std::string &name, const veViewport &vp)
{
	auto camera = new veOctreeCamera(vp);
	camera->setName(name);
	camera->setSceneManager(this);
	_cameraList.push_back(camera);
	return camera;
}

void veOctreeSceneManager::requestRender(veNode *node)
{
	veOctreeNode *ocNode = static_cast<veOctreeNode *>(node);
	if (!ocNode) return;
	_threadPool.enqueue(nullptr, nullptr, [this, ocNode] {
		if (!ocNode->octant) {
			if (!ocNode->isIn(this->_octree->boundingBox)) {
				this->_octree->addNode(ocNode);
			}
			else {
				addOctreeNode(ocNode, this->_octree);
			}
		}
		else {
			if (!ocNode->isIn(ocNode->octant->boundingBox)) {
				ocNode->octant->removeNode(ocNode);

				if (!ocNode->isIn(this->_octree->boundingBox)) {
					this->_octree->addNode(ocNode);
				}
				else {
					addOctreeNode(ocNode, this->_octree);
				}
			}
		}
	});
}

void veOctreeSceneManager::requestRayCast(veRay *ray)
{
	enqueueRequest([this, ray] {
		this->intersectByRay(_octree, ray);
		veSceneManager::requestRayCast(ray);
	});
}

bool veOctreeSceneManager::isNodeVisibleInScene(veNode *node)
{
	veOctreeNode *ocNode = static_cast<veOctreeNode *>(node);
	for (auto &cam : _cameraList) {
		if (cam->isInScene()) {
			veOctreeCamera *octreeCam = static_cast<veOctreeCamera *>(cam.get());
			if (octreeCam->isNodeVisibleInCamera(ocNode))
				return true;
		}
	}
	return false;
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
			veVec3 &min = octree->originBoundingBox.min();
			veVec3 &max = octree->originBoundingBox.max();
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
			octree->boundingBox = octree->originBoundingBox;
			octant->setChild(x, y, z, octree);	
		}

		addOctreeNode(node, octant->getChild(x, y, z), ++depth);
	}
	else {
		octant->addNode(node);
	}
}

void veOctreeSceneManager::intersectByRay(veOctree *octant, veRay *ray)
{
	if (!octant) return;
	if (ray->isIntersectWith(octant->boundingBox)) {
		if (!octant->nodeList.empty()) {
			std::unique_lock<std::mutex> lock(octant->nodeListMutex);
			for (auto &iter : octant->nodeList) {
				if (iter->isVisible()) {
					if (ray->isIntersectWith(iter->getBoundingBox())) {
						for (unsigned int i = 0; i < iter->getRenderableObjectCount(); ++i) {
							if (iter->getRenderableObject(i)->isVisible())
								iter->getRenderableObject(i)->intersectWith(ray, iter);
						}
					}
				}
			}
		}

		for (unsigned int i = 0; i < 8; ++i) {
			if (octant->children[i]) {
				intersectByRay(octant->children[i], ray);
			}
		}
	}
}

void veOctreeSceneManager::update()
{
	_root->update(this, veMat4::IDENTITY);
	_octree->updateBoundingBox();
}

void veOctreeSceneManager::render()
{
	//culling();
	veSceneManager::setContextCurrent();
	auto mainCamera = _visualiser->getCamera();
	for (auto &iter : _cameraList) {
		if (iter->isVisible() && iter->isInScene() && iter != mainCamera) {
			if (iter->getFrameBufferObject()) {
				veOctreeCamera *rttCam = static_cast<veOctreeCamera *>(iter.get());
				rttCam->walkingOctree(this->_octree);
				rttCam->render();
			}
		}
	}

	if (mainCamera && mainCamera->isInScene() && mainCamera->isVisible()) {
		veOctreeCamera *mainCam = static_cast<veOctreeCamera *>(mainCamera);
		mainCam->walkingOctree(this->_octree);
		mainCam->render();
	}
	veSceneManager::render();
}

void veOctreeSceneManager::culling()
{
	//for (auto &iter : _cameraList) {
	//	if (iter->isVisible() && iter->isInScene()) {
	//		veOctreeCamera *cam = static_cast<veOctreeCamera *>(iter);
	//		//_threadPool.enqueue(nullptr, nullptr, [this, cam] {
	//			cam->walkingOctree(this->_octree);
	//		//});
	//	}
	//}
}