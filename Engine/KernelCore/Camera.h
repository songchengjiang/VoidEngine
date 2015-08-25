#ifndef _VE_CAMERA_
#define _VE_CAMERA_
#include "Prerequisites.h"
#include "BaseCore/Matrix4.h"

class VE_EXPORT veCamera
{
public:
	veCamera();
	~veCamera();

	USE_VE_PTR

	void setProjectionMatrixAsOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
	void setProjectionMatrixAsPerspective(float fovy, float aspectRatio, float zNear, float zFar);
	veMat4& projectionMatrix() { return _projectionMat; }

	void setViewMatrixAslookAt(const veVec3 &eye, const veVec3 &center, const veVec3 &up);
	veMat4& viewMatrix() { return _viewMat; }

private:

	veMat4 _projectionMat;
	veMat4 _viewMat;
};

#endif
