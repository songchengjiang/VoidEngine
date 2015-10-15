#ifndef _VE_OCTREE_NODE_
#define _VE_OCTREE_NODE_
#include "Node.h"
#include "Octree.h"

class veOctreeNode : public veNode
{
public:
	veOctreeNode();
	~veOctreeNode();

	void setOctant(veOctree *octant) { _octant = octant; }
	veOctree* getOctant() { return _octant; }
	bool isIn(const veBoundingBox & bbox);

private:

	veOctree *_octant;
};

#endif