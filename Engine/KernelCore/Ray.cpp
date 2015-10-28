#include "Ray.h"
#include "SceneManager.h"

veRay::veRay(const veVec3 &start, const veVec3 &end)
	: USE_VE_PTR_INIT
	, _start(start)
	, _end(end)
{
	_dir = (_end - _start);
	_dir.normalize();
}

veRay::veRay()
	: USE_VE_PTR_INIT
{

}

veRay::~veRay()
{

}

void veRay::setStart(const veVec3 &start)
{
	_start = start;
	_dir = (_end - _start);
	_dir.normalize();
}

void veRay::setEnd(const veVec3 &end)
{
	_end = end;
	_dir = (_end - _start);
	_dir.normalize();
}

void veRay::apply(veSceneManager *sm, const RayCallback &callBack)
{
	_intersections.clear();
	_callBack = callBack;
	sm->requestRayCast(this);
}

void veRay::addIntersection(const Intersection &inters)
{
	_intersections.push_back(inters);
}

bool veRay::isIntersectWith(const veBoundingBox &bbox)
{
	if (bbox.isNull()) return false;
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

bool veRay::isIntersectWith(const veVec3 &p0, const veVec3 &p1, const veVec3 &p2, veVec3 &intersectPoint, veVec3 &intersectNormal, bool isCullingBack)
{
	veVec3 e1 = p1 - p0;
	veVec3 e2 = p2 - p0;
	veVec3 n = e1.crossProduct(e2);
	if (isCullingBack) {
		if (0 < (_end - p0).dotProduct(n))
			return false;
	}
	veReal a = n.dotProduct(_dir);
	if (-1e-5 < a && a < 1e-5) return false;
	veReal f = -1.0 / a;
	veVec3 s = _start - p0;
	veVec3 m = s.crossProduct(_dir);
	veReal u = f * m.dotProduct(e2);
	if (u < 0.0f) return false;
	veReal v = -f * m.dotProduct(e1);
	if (v < 0.0f || 1.0 < (u + v)) return false;
	veReal t = f * n.dotProduct(s);
	intersectPoint = _start + t * _dir;
	intersectNormal = n;
	intersectNormal.normalize();
	return true;
}
