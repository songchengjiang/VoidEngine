#ifndef _VE_PLANE_
#define _VE_PLANE_
#include "Prerequisites.h"
#include "BaseCore/Vector3.h"

class VE_EXPORT vePlane
{
public:

	enum Side
	{
		NO_SIDE,
		POSITIVE_SIDE,
		NEGATIVE_SIDE,
		BOTH_SIDE
	};

	vePlane();
	vePlane(const veVec3 &norm, veReal d);
	~vePlane();

	const veVec3& normal() const { return _normal; }
	veVec3& normal() { return _normal; }
	const veReal& constantD() const { return _d; }
	veReal& constantD() { return _d; }

	veReal getDistance(const veVec3 &point) const;

	Side getSide(const veVec3 &point) const;
	Side getSide(const veVec3 &center, const veVec3 &halfSize) const;

private:

	veVec3 _normal;
	veReal _d;
};

#endif