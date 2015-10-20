#ifndef _VE_OCTREE_NODE_
#define _VE_OCTREE_NODE_
#include "Node.h"
#include "Octree.h"

class VE_EXPORT veOctreeNode : public veNode
{
public:
	veOctreeNode();
	~veOctreeNode();

	bool isIn(const veBoundingBox & bbox);

	veOctree *octant;
	bool isVisibleInOctant;
};

#endif