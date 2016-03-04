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

void veOctreeCamera::cull(veNodeList &visibledNodeList)
{
	traverseOctree(_octree, visibledNodeList);
}

void veOctreeCamera::traverseOctree(veOctree *octant, veNodeList &visibledNodeList)
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
					visibledNodeList.push_back(iter);
				}
			}
		}
	}

	for (unsigned int i = 0; i < 8; ++i) {
		if (octant->children[i]) {
			traverseOctree(octant->children[i], visibledNodeList);
		}
	}
}
