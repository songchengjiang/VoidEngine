#include "OctreeNode.h"

veOctreeNode::veOctreeNode()
	: octant(nullptr)
{

}

veOctreeNode::~veOctreeNode()
{

}

bool veOctreeNode::removeChild(veNode *child)
{
    veOctreeNode *octreeChild = dynamic_cast<veOctreeNode *>(child);
    if (octreeChild) {
        if (octreeChild->octant)
            octreeChild->octant->removeNode(octreeChild);
    }
    return veNode::removeChild(child);
}

veNode* veOctreeNode::removeChild(size_t cIndex)
{
    auto child = veNode::removeChild(cIndex);
    veOctreeNode *octreeChild = dynamic_cast<veOctreeNode *>(child);
    if (octreeChild) {
        if (octreeChild->octant)
            octreeChild->octant->removeNode(octreeChild);
    }
    return child;
}

bool veOctreeNode::isIn(const veBoundingBox & bbox)
{
	if (bbox.isNull() || !_isInScene) return false;

	veVec3 center = _boundingBox.center();
	veVec3 bmin = bbox.min();
	veVec3 bmax = bbox.max();
	if (!(bmax >= center && bmin <= center)) return false;

	veVec3 octreeSize = bmax - bmin;
	veVec3 nodeSize = _boundingBox.max() - _boundingBox.min();

	return nodeSize.x() < octreeSize.x() && nodeSize.y() < octreeSize.y() && nodeSize.z() < octreeSize.z();
}