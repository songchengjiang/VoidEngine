#ifndef _VE_OCTREE_CAMERA_
#define _VE_OCTREE_CAMERA_
#include "Camera.h"

class VE_EXPORT veOctreeCamera : public veCamera
{
public:
	veOctreeCamera();
	veOctreeCamera(const veViewport &vp);
	~veOctreeCamera();

};

#endif