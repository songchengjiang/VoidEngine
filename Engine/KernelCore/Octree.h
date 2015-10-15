#ifndef _VE_OCTREE_
#define _VE_OCTREE_
#include <vector>
#include "BoudingBox.h"

class veNode;
class veOctree
{
public:
	veOctree();
	~veOctree();

	inline void addNode(veNode *node);
	inline void removeNode(veNode *node);

	inline void setChild(unsigned int idx, veOctree *child);
	veOctree* getParent() { return _parent; }

	veBoundingBox boundingBox;

private:

	typedef std::vector<veNode *> NodeList;
	NodeList  _nodeList;
	veOctree* _children[8];
	veOctree* _parent;
};

#endif