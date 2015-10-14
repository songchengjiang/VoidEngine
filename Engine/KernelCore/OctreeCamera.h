#ifndef _VE_OCTREE_CAMERA_
#define _VE_OCTREE_CAMERA_
#include "Camera.h"

class veOctreeCamera : public veCamera
{
public:
	veOctreeCamera();
	veOctreeCamera(const veViewport &vp);
	~veOctreeCamera();

private:

};

#endif