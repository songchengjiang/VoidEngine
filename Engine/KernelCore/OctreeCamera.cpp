#include "OctreeCamera.h"
#include "OctreeRenderQueue.h"
#include "OctreeNode.h"
#include "Octree.h"

veOctreeCamera::veOctreeCamera()
{
	_renderQueue = new veOctreeRenderQueue;
}

veOctreeCamera::veOctreeCamera(const veViewport &vp)
	: veCamera(vp)
{
	_renderQueue = new veOctreeRenderQueue;
}

veOctreeCamera::~veOctreeCamera()
{

}

void veOctreeCamera::render()
{
	if (!_visibleOctreeNodeList.empty()) {
		{
			std::unique_lock<std::mutex> lock(_visitMutex);
			for (auto &iter : _visibleOctreeNodeList) {
				for (unsigned int i = 0; i < iter->getRenderableObjectCount(); ++i) {
					if (iter->getRenderableObject(i)->isVisible())
						iter->getRenderableObject(i)->render(iter, this);
				}
			}
		}
		veCamera::render();
	}
}

void veOctreeCamera::walkingOctree(veOctree *octree)
{
	{
		std::unique_lock<std::mutex> lock(_visitMutex);
		if (!_visibleOctreeNodeList.empty())
			_visibleOctreeNodeList.clear();
	}
	traverseOctree(octree);
}

bool veOctreeCamera::isNodeVisibleInCamera(veOctreeNode *node)
{
	std::unique_lock<std::mutex> lock(_visitMutex);
	auto iter = std::find(_visibleOctreeNodeList.begin(), _visibleOctreeNodeList.end(), node);
	if (iter != _visibleOctreeNodeList.end())
		return true;
	return false;
}

void veOctreeCamera::traverseOctree(veOctree *octant)
{
	if (isOutOfFrustum(octant->boundingBox)) {
		return;
	}

	if (!octant->nodeList.empty()) {
		for (auto &iter : octant->nodeList) {
			if (iter->isVisible() && (iter->getMask() & getMask())) {
				if (!isOutOfFrustum(iter->getBoundingBox())) {
					std::unique_lock<std::mutex> lock(_visitMutex);
					_visibleOctreeNodeList.push_back(static_cast<veOctreeNode *>(iter));
				}
			}
		}
	}

	for (unsigned int i = 0; i < 8; ++i) {
		if (octant->children[i]) {
			traverseOctree(octant->children[i]);
		}
	}
}
