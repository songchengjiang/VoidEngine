#include "OctreeCamera.h"
#include "OctreeRenderQueue.h"
#include "OctreeNode.h"
#include "Octree.h"
#include "RenderPipeline.h"
#include <algorithm>

veOctreeCamera::veOctreeCamera(veSceneManager *sm)
	: veCamera(sm)
	, _octree(nullptr)
{
	_renderQueue = new veOctreeRenderQueue;
}

veOctreeCamera::veOctreeCamera(veSceneManager *sm, const veViewport &vp)
	: veCamera(sm, vp)
	, _octree(nullptr)
{
	_renderQueue = new veOctreeRenderQueue;
}

veOctreeCamera::~veOctreeCamera()
{

}

void veOctreeCamera::cull(veRenderPipeline *renderPipeline)
{
	std::unique_lock<std::mutex> lock(_visitMutex);
	if (!_visibleNodeList.empty())
		_visibleNodeList.clear();
	traverseOctree(_octree, renderPipeline);
}

void veOctreeCamera::fillRenderQueue()
{
	std::unique_lock<std::mutex> lock(_visitMutex);
	if (!_visibleNodeList.empty()) {
		{
			for (auto &iter : _visibleNodeList) {
				for (unsigned int i = 0; i < iter->getRenderableObjectCount(); ++i) {
					if (iter->getRenderableObject(i)->isVisible())
						iter->getRenderableObject(i)->render(iter, this);
				}
			}
		}
	}
}

bool veOctreeCamera::hasDynamicNodeVisibleInCamera()
{
	std::unique_lock<std::mutex> lock(_visitMutex);
	if (!_visibleNodeList.empty()) {
		{
			for (auto &iter : _visibleNodeList) {
				if (iter->isDynamicNode())
					return true;
			}
		}
	}
	return false;
}

bool veOctreeCamera::isNodeVisibleInCamera(veOctreeNode *node)
{
	std::unique_lock<std::mutex> lock(_visitMutex);
	auto iter = std::find(_visibleNodeList.begin(), _visibleNodeList.end(), node);
	if (iter != _visibleNodeList.end())
		return true;
	return false;
}

void veOctreeCamera::traverseOctree(veOctree *octant, veRenderPipeline *renderPipeline)
{
	if (isOutOfFrustum(octant->boundingBox)) {
		return;
	}

	if (!octant->nodeList.empty()) {
		std::unique_lock<std::mutex> lock(octant->nodeListMutex);
		for (auto &iter : octant->nodeList) {
			if (iter->isVisible() && (iter->getMask() & getMask())) {
				if (!isOutOfFrustum(iter->getBoundingBox())) {
					//std::unique_lock<std::mutex> lock(_visitMutex);
					//_visibleNodeList.push_back(iter);
					renderPipeline->pushVisibleNode(this, iter);
				}
			}
		}
	}

	for (unsigned int i = 0; i < 8; ++i) {
		if (octant->children[i]) {
			traverseOctree(octant->children[i], renderPipeline);
		}
	}
}
