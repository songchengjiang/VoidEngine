#include "Ray.h"
#include "SceneManager.h"

veRay::veRay(const veVec3 &start, const veVec3 &end)
	: USE_VE_PTR_INIT
	, _start(start)
	, _end(end)
    , _isDicardBackFace(true)
{
	_dir = (_end - _start);
	_dir.normalize();
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

void veRay::apply(veSceneManager *sm, bool isSortResults)
{
	_intersections.clear();
	sm->requestRayCast(this);
    if (isSortResults){
        std::sort(_intersections.begin(), _intersections.end(), [this](const Intersection &left, const Intersection &right) -> bool {
            veReal leftDis2 = (left.worldPosition - _start).squaredLength();
            veReal rightDis2 = (right.worldPosition - _start).squaredLength();
            return leftDis2 < rightDis2;
        });
    }
}

void veRay::addIntersection(const Intersection &inters)
{
	_intersections.push_back(inters);
}

bool veRay::isIntersectWith(const veVec3 &p0, const veVec3 &p1, veVec3 *intersectPoint, veReal errRange2)
{
    veReal f1ab = (_end - _start).squaredLength();
    veReal f1cd = (p1 - p0).squaredLength();
    veReal f2 = (_end.x() - _start.x()) * (p1.x() - p0.x()) +
                (_end.y() - _start.y()) * (p1.y() - p0.y()) +
                (_end.z() - _start.z()) * (p1.z() - p0.z());
    veReal f3ab = (_end.x() - _start.x()) * (p0.x() - _start.x()) +
                (_end.y() - _start.y()) * (p0.y() - _start.y()) +
                (_end.z() - _start.z()) * (p0.z() - _start.z());
    veReal f3cd = (p1.x() - p0.x()) * (p0.x() - _start.x()) +
                (p1.y() - p0.y()) * (p0.y() - _start.y()) +
                (p1.z() - p0.z()) * (p0.z() - _start.z());
    
    veReal t1 = (f3ab * f1cd - f3cd * f2) / (f1ab * f1cd - f2 * f2);
    if (t1 < 0.0f || 1.0f < t1) return false;
    veReal t2 = (f3cd * f1ab - f2 * f3ab) / (f2 * f2 - f1ab * f1cd);
    if (t2 < 0.0f || 1.0f < t2) return false;
    
    veVec3 m = (1.0f - t1) * _start + t1 * _end;
    veVec3 n = (1.0f - t2) * p0 + t2 * p1;
    if (errRange2 < (m - n).squaredLength()) return false;
    if (intersectPoint != nullptr){
        *intersectPoint = n;
    }
    return true;
}

bool veRay::isIntersectWith(const veBoundingBox &bbox)
{
	if (bbox.isNull()) return false;
	veVec3 c = (_start + _end) * 0.5f;
	veVec3 w = (_end - _start) * 0.5f;
	veVec3 h = (bbox.max() - bbox.min()) * 0.5f;
	c = c - bbox.center();
	veReal vx = veMath::veAbs(w.x());
	veReal vy = veMath::veAbs(w.y());
	veReal vz = veMath::veAbs(w.z());
	if ((vx + h.x()) < veMath::veAbs(c.x())) 
		return false;
	if ((vy + h.y()) < veMath::veAbs(c.y())) 
		return false;
	if ((vz + h.z()) < veMath::veAbs(c.z()))
		return false;
	if ((h.y() * vz + h.z() * vy) < veMath::veAbs(c.y() * w.z() - c.z() * w.y())) 
		return false;
	if ((h.x() * vz + h.z() * vx) < veMath::veAbs(c.x() * w.z() - c.z() * w.x())) 
		return false;
	if ((h.x() * vy + h.y() * vx) < veMath::veAbs(c.x() * w.y() - c.y() * w.x())) 
		return false;
	return true;
}

bool veRay::isIntersectWith(const vePlane &plane, veVec3 *intersectPoint)
{
    veReal s = plane.normal().dotProduct(_start) + plane.constantD();
    veReal e = plane.normal().dotProduct(_end) + plane.constantD();
    if (0.0f < s * e) return false;
    veReal dirDotN = _dir.dotProduct(plane.normal());
    if (dirDotN == 0.0f) return false;
    veReal t = -s / dirDotN;
    if (intersectPoint != nullptr){
        *intersectPoint = _start + t * _dir;
    }
    return true;
}

bool veRay::isIntersectWith(const veVec3 &p0, const veVec3 &p1, const veVec3 &p2, veVec3 *intersectPoint, veVec3 *intersectNormal)
{
	veVec3 e1 = p1 - p0;
	veVec3 e2 = p2 - p0;
	veVec3 n = e1.crossProduct(e2);
	if (_isDicardBackFace) {
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
    if (intersectPoint != nullptr){
        *intersectPoint = _start + t * _dir;
    }
    if (intersectNormal != nullptr){
        *intersectNormal = n;
        (*intersectNormal).normalize();
    }
	return true;
}
