#ifndef _VE_OCTREE_CAMERA_
#define _VE_OCTREE_CAMERA_
#include "Camera.h"
#include <mutex>

class veOctreeNode;
class veOctree;
class VE_EXPORT veOctreeCamera : public veCamera
{
public:
	veOctreeCamera(veSceneManager *sm);
	veOctreeCamera(veSceneManager *sm, const veViewport &vp);
	~veOctreeCamera();

	virtual void cull(veRenderPipeline *renderPipeline) override;
	virtual void fillRenderQueue() override;
	virtual bool hasDynamicNodeVisibleInCamera() override;

	bool isNodeVisibleInCamera(veOctreeNode *node);

	void setOctree(veOctree *octree) { _octree = octree; }

protected:

	void traverseOctree(veOctree *octant, veRenderPipeline *renderPipeline);

protected:

	std::mutex _visitMutex;
	veOctree *_octree;
};

#endif