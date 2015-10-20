#include "Ray.h"
#include "SceneManager.h"

veRay::veRay(const veVec3 &start, const veVec3 &end)
	: USE_VE_PTR_INIT
	, _start(start)
	, _end(end)
{

}

veRay::~veRay()
{

}

void veRay::apply(veSceneManager *sm)
{
	_intersections.clear();
	sm->requestRayCast(this);
}

void veRay::addIntersection(const Intersection &inters)
{
	_intersections.push_back(inters);
}

bool veRay::isIntersectWith(const veBoundingBox &bbox)
{
	veVec3 rayCenter = (_start + _end) * 0.5f;
	veVec3 rayHalfSize = (_end - _start) * 0.5f;
	veVec3 bboxHalfSize = (bbox.max() - bbox.min()) * 0.5f;
	rayCenter = rayCenter - bbox.center();
	veReal vx = veMath::abs(rayHalfSize.x());
	veReal vy = veMath::abs(rayHalfSize.y());
	veReal vz = veMath::abs(rayHalfSize.z());
	if (vx + bboxHalfSize.x() < veMath::abs(rayCenter.x())) 
		return false;
	if (vx + bboxHalfSize.y() < veMath::abs(rayCenter.y())) 
		return false;
	if (vx + bboxHalfSize.z() < veMath::abs(rayCenter.z()))
		return false;
	if (bboxHalfSize.y() * vz + bboxHalfSize.z() * vy < veMath::abs(rayCenter.y() * rayHalfSize.z() - rayCenter.z() * rayHalfSize.y())) 
		return false;
	if (bboxHalfSize.x() * vz + bboxHalfSize.z() * vx < veMath::abs(rayCenter.x() * rayHalfSize.z() - rayCenter.z() * rayHalfSize.x())) 
		return false;
	if (bboxHalfSize.x() * vy + bboxHalfSize.y() * vx < veMath::abs(rayCenter.x() * rayHalfSize.y() - rayCenter.y() * rayHalfSize.x())) 
		return false;
	return true;
}

bool veRay::isIntersectWith(const veVec3 &p1, const veVec3 &p2, const veVec3 &p3, veVec3 *intersectPoint, veVec3 *intersectNormal)
{
	return false;
}
