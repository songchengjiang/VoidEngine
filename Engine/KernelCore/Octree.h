#ifndef _VE_OCTREE_
#define _VE_OCTREE_
#include "Prerequisites.h"
#include "BoudingBox.h"
#include <mutex>

class veOctreeNode;
/*
veOctree:
 / 4 - 5 /
/ 6 - 7 /
./ 0 - 1 /
/ 2 - 3 /
*/
class VE_EXPORT veOctree
{
public:
	veOctree();
	~veOctree();

    void addNode(veOctreeNode *node);
    void removeNode(veOctreeNode *node);

    void setChild(unsigned int x, unsigned int y, unsigned int z, veOctree *child);
	veOctree* getChild(unsigned int x, unsigned int y, unsigned int z);
	void getChildIndex(const veBoundingBox &bbox, unsigned int &x, unsigned int &y, unsigned int &z);

	bool isTwiceSize(const veBoundingBox &bbox);

	bool updateBoundingBox();

	veBoundingBox boundingBox;
	veBoundingBox originBoundingBox;
	veOctree* children[8];

	typedef std::vector<veOctreeNode *> OctreeNodeList;
	OctreeNodeList  nodeList;
	veOctree* parent;
	std::mutex nodeListMutex;

private:

	bool _needUpdateBoundingBox;
};

#endif