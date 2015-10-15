#include "OctreeNode.h"

veOctreeNode::veOctreeNode()
	: _octant(nullptr)
{

}

veOctreeNode::~veOctreeNode()
{

}

bool veOctreeNode::isIn(const veBoundingBox & bbox)
{
	if (bbox.isNull()) return false;

	return true;
}