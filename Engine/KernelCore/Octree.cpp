#include "Octree.h"
#include "OctreeNode.h"

veOctree::veOctree()
	: parent(nullptr)
	, _needUpdateBoundingBox(true)
{
	memset(children, 0, sizeof(veOctree*) * 8);
}

veOctree::~veOctree()
{
	for (unsigned int i = 0; i < 8; ++i) {
		delete children[i];
	}
}

void veOctree::addNode(veOctreeNode *node)
{
	std::unique_lock<std::mutex> lock(nodeListMutex);
	node->octant = this;
	nodeList.push_back(node);
	_needUpdateBoundingBox = true;
}

void veOctree::removeNode(veOctreeNode *node)
{
	std::unique_lock<std::mutex> lock(nodeListMutex);
	auto iter = std::find(nodeList.begin(), nodeList.end(), node);
	if (iter == nodeList.end()) return;
	(*iter)->octant = nullptr;
	nodeList.erase(iter);
	_needUpdateBoundingBox = true;
}

void veOctree::setChild(unsigned int x, unsigned int y, unsigned int z, veOctree *child)
{
	unsigned int idx = y * 4 + z * 2 + x;
	if (children[idx]) 
		children[idx]->parent = nullptr;
	children[idx] = child;
	child->parent = this;
}

veOctree* veOctree::getChild(unsigned int x, unsigned int y, unsigned int z)
{
	return children[y * 4 + z * 2 + x];
}

void veOctree::getChildIndex(const veBoundingBox &bbox, unsigned int &x, unsigned int &y, unsigned int &z)
{
	veVec3 center = (boundingBox.max() + boundingBox.min()) * 0.5f;
	veVec3 ncenter = (bbox.max() + bbox.min()) * 0.5f;;

	if (center.x() < ncenter.x())
		x = 1;
	else
		x = 0;

	if (center.y() < ncenter.y())
		y = 1;
	else
		y = 0;

	if (center.z() < ncenter.z())
		z = 1;
	else
		z = 0;
}

bool veOctree::isTwiceSize(const veBoundingBox &bbox)
{
	veVec3 halfBoxSize = (boundingBox.max() - boundingBox.min()) * 0.5f;
	veVec3 boxSize = bbox.max() - bbox.min();
	return boxSize <= halfBoxSize;
}

bool veOctree::updateBoundingBox()
{
	bool state = false;
	if (_needUpdateBoundingBox) {
		std::unique_lock<std::mutex> lock(nodeListMutex);
		boundingBox = originBoundingBox;
		for (auto &iter : nodeList) {
			boundingBox.expandBy(iter->getBoundingBox());
		}
		state = true;
		_needUpdateBoundingBox = false;
	}

	for (unsigned int i = 0; i < 8; ++i) {
		if (children[i]) {
			if (children[i]->updateBoundingBox()) {
				boundingBox.expandBy(children[i]->boundingBox);
				state = true;
			}
		}
	}

	return state;
}
