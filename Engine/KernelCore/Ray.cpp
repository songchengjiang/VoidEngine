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
	veVec3 c = (_start + _end) * 0.5f;
	veVec3 w = (_end - _start) * 0.5f;
	veVec3 h = (bbox.max() - bbox.min()) * 0.5f;
	c = c - bbox.center();
	veReal vx = veMath::abs(w.x());
	veReal vy = veMath::abs(w.y());
	veReal vz = veMath::abs(w.z());
	if ((vx + h.x()) < veMath::abs(c.x())) 
		return false;
	if ((vy + h.y()) < veMath::abs(c.y())) 
		return false;
	if ((vz + h.z()) < veMath::abs(c.z()))
		return false;
	if ((h.y() * vz + h.z() * vy) < veMath::abs(c.y() * w.z() - c.z() * w.y())) 
		return false;
	if ((h.x() * vz + h.z() * vx) < veMath::abs(c.x() * w.z() - c.z() * w.x())) 
		return false;
	if ((h.x() * vy + h.y() * vx) < veMath::abs(c.x() * w.y() - c.y() * w.x())) 
		return false;
	return true;
}

bool veRay::isIntersectWith(const veVec3 &p1, const veVec3 &p2, const veVec3 &p3, veVec3 *intersectPoint, veVec3 *intersectNormal)
{
	return false;
}
