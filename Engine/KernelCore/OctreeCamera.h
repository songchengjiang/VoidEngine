#ifndef _VE_OCTREE_CAMERA_
#define _VE_OCTREE_CAMERA_
#include "Camera.h"

class veOctreeNode;
class veOctree;
class VE_EXPORT veOctreeCamera : public veCamera
{
public:
	veOctreeCamera();
	veOctreeCamera(const veViewport &vp);
	~veOctreeCamera();

	void render(veOctree *octree);
	virtual void render() override;

	std::vector<veOctreeNode *> visibleOctreeNodeList;

protected:

	void traverseOctree(veOctree *octant);
};

#endif