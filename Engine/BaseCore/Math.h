#ifndef _VE_MATH_
#define _VE_MATH_
#include "Prerequisites.h"
#include <math.h>

class VE_EXPORT veMath
{
public:
	static const veReal PI;
	static const veReal TWO_PI;
	static const veReal HALF_PI;
	static const veReal QUARTER_PI;

	veMath();
	~veMath();

	static veReal sqrt(veReal val) { return ::sqrt(val); }
	static veReal sqr(veReal val) { return val * val; }
	static veReal sin(veReal val) { return ::sin(val); }
	static veReal cos(veReal val) { return ::cos(val); }
	static veReal tan(veReal val) { return ::tan(val); }
	static veReal asin(veReal val) { return ::asin(val); }
	static veReal acos(veReal val) { return ::acos(val); }
	static veReal atan(veReal val) { return ::atan(val); }
	static veReal atan2(veReal y, veReal x) { return ::atan2(y, x); }
	static veReal abs(veReal val) { return ::fabs(val); }
	static veReal radian(veReal deg) { return deg * PI / 180.0f; }
	static veReal degree(veReal rad) { return rad * 180.0f / PI; }
	static veReal randomUnitization() { return (veReal)rand() / RAND_MAX; }
	static bool isNaN(veReal val) { return val != val; }
};

#endif
