#ifndef _VE_OCTREE_DEBUGER_
#define _VE_OCTREE_DEBUGER_
#include "Debuger.h"

class veOctree;
class VE_EXPORT veOctreeDebuger : public veDebuger
{
public:
	veOctreeDebuger(veSceneManager *sm);
	~veOctreeDebuger();

	virtual void render(veNode *node, veCamera *camera, unsigned int contextID) override;

	void debugDrawOctree(bool isDraw, const veVec4 &color = veVec4::WHITE) {
		_isDrawOctree = isDraw; 
		_drawDrawOctreeColor = color;
	}

protected:

	void renderOctree(veOctree *octree);

protected:

	bool _isDrawOctree;
	veVec4 _drawDrawOctreeColor;
};

#endif