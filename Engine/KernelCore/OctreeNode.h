#ifndef _VE_OCTREE_NODE_
#define _VE_OCTREE_NODE_
#include "Node.h"
#include "Octree.h"

class VE_EXPORT veOctreeNode : public veNode
{
public:
	veOctreeNode();
	~veOctreeNode();
    
    virtual bool removeChild(veNode *child) override;
    virtual veNode* removeChild(size_t cIndex) override;

	bool isIn(const veBoundingBox & bbox);

	veOctree *octant;
};

#endif