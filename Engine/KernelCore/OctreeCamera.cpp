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
	if (!visibleOctreeNodeList.empty()) {
		for (auto &iter : visibleOctreeNodeList) {
			for (unsigned int i = 0; i < iter->getRenderableObjectCount(); ++i) {
				if (iter->getRenderableObject(i)->isVisible())
					iter->getRenderableObject(i)->render(iter, this);
			}
		}
		veCamera::render();
	}
}

void veOctreeCamera::render(veOctree *octree)
{
	if (!visibleOctreeNodeList.empty())
		visibleOctreeNodeList.clear();
	traverseOctree(octree);
	render();
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
					visibleOctreeNodeList.push_back(static_cast<veOctreeNode *>(iter));
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
