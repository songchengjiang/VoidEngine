#include "Plane.h"
#include "BaseCore/MathAlgorithm.h"

veReal AbsDotProduct(const veVec3 &lval, const veVec3 &rval) {
	return veMath::veAbs(lval.x() * rval.x()) + veMath::veAbs(lval.y() * rval.y()) + veMath::veAbs(lval.z() * rval.z());
}

vePlane::vePlane()
{

}

vePlane::vePlane(const veVec3 &norm, veReal d)
	: _normal(norm)
	, _d(d)
{

}

vePlane::~vePlane()
{

}

veReal vePlane::getDistance(const veVec3 &point) const
{
	return _normal.dotProduct(point) + _d;
}

vePlane::Side vePlane::getSide(const veVec3 &center, const veVec3 &halfSize) const
{
	veReal dist = getDistance(center);

	veReal maxAbsDist = AbsDotProduct(_normal, halfSize);
	if (dist < -maxAbsDist)
		return vePlane::NEGATIVE_SIDE;

	if (dist > +maxAbsDist)
		return vePlane::POSITIVE_SIDE;

	return vePlane::BOTH_SIDE;
}

vePlane::Side vePlane::getSide(const veVec3 &point) const
{
	veReal fDistance = getDistance(point);

	if (fDistance < 0.0)
		return vePlane::NEGATIVE_SIDE;

	if (fDistance > 0.0)
		return vePlane::POSITIVE_SIDE;

	return vePlane::NO_SIDE;
}
