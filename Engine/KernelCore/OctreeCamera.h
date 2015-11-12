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

	void renderingOctree();
	void walkingOctree(veOctree *octree);

	bool isNodeVisibleInCamera(veOctreeNode *node);

protected:

	void traverseOctree(veOctree *octant);

protected:

	std::vector<veOctreeNode *> _visibleOctreeNodeList;
	std::mutex _visitMutex;
};

#endif