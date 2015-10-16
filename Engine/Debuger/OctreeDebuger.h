#ifndef _VE_OCTREE_DEBUGER_
#define _VE_OCTREE_DEBUGER_
#include "Debuger.h"

class veOctree;
class VE_EXPORT veOctreeDebuger : public veDebuger
{
public:
	veOctreeDebuger();
	~veOctreeDebuger();

	virtual void update(veNode *node, veSceneManager *sm) override;

	void debugDrawOctree(bool isDraw) { _isDrawOctree = isDraw; }

protected:

	void renderOctree(veOctree *octree);

protected:

	bool _isDrawOctree;
};

#endif