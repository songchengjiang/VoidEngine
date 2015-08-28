#include "Camera.h"

veCamera::veCamera()
	: USE_VE_PTR_INIT
	, _viewMat(veMat4::IDENTITY)
	, _projectionMat(veMat4::IDENTITY)
{

}

veCamera::~veCamera()
{

}

void veCamera::setProjectionMatrixAsOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	_projectionMat.set(2.0f / (right - left), 0.0f                 , 0.0f                 , -(right + left) / (right - left)
		             , 0.0f                 , 2.0f / (top - bottom), 0.0f                 , -(top + bottom) / (top - bottom)
		             , 0.0f                 , 0.0f                 , 2.0f / (zNear - zFar), (zNear + zFar) / (zNear - zFar)
		             , 0.0f                 , 0.0f                 , 0.0f                 , 1.0f);
}

void veCamera::setProjectionMatrixAsPerspective(float fovy, float aspectRatio, float zNear, float zFar)
{
	float top = zNear * tan(veMath::radian(fovy * 0.5f));
	float bottom = -top;
	float right = top * aspectRatio;
	float left = -right;
	float a = -(zFar + zNear) / (zFar - zNear);
	float b = -(2.0 * zNear * zFar) / (zFar - zNear);

	_projectionMat.set((2.0f * zNear) / (right - left), 0.0f                           , (right + left) / (right - left), 0.0f
		              , 0.0f                          , (2.0f * zNear) / (top - bottom), (top + bottom) / (top - bottom), 0.0f
		              , 0.0f                          , 0.0f                           , a                              , b
		              , 0.0f                          , 0.0f                           , -1.0f                          , 0.0f);
}

void veCamera::setViewMatrixAslookAt(const veVec3 &eye, const veVec3 &center, const veVec3 &up)
{
	veVec3 f = eye - center;
	f.normalize();
	veVec3 s = up.crossProduct(f);
	s.normalize();
	veVec3 u = f.crossProduct(s);
	if (f.isZeroLength() || s.isZeroLength() || u.isZeroLength()) return;
	_viewMat.set(s.x(), s.y(), s.z(), -s.dotProduct(eye)
		       , u.x(), u.y(), u.z(), -u.dotProduct(eye)
		       , f.x(), f.y(), f.z(), -f.dotProduct(eye)
		       , 0.0f , 0.0f , 0.0f , 1.0f);
}
