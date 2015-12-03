#ifndef _VE_OCTREE_CAMERA_
#define _VE_OCTREE_CAMERA_
#include "Camera.h"
#include <mutex>

class veOctreeNode;
class veOctree;
class VE_EXPORT veOctreeCamera : public veCamera
{
public:
	veOctreeCamera();
	veOctreeCamera(const veViewport &vp);
	~veOctreeCamera();

	virtual void cull() override;
	virtual void fillRenderQueue() override;

	bool isNodeVisibleInCamera(veOctreeNode *node);

	void setOctree(veOctree *octree) { _octree = octree; }

protected:

	void traverseOctree(veOctree *octant);

protected:

	std::vector<veOctreeNode *> _visibleOctreeNodeList;
	std::mutex _visitMutex;
	veOctree *_octree;
};

#endif