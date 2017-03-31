#ifndef _VE_MATH_
#define _VE_MATH_
#include "Prerequisites.h"
#include <math.h>
#include <stdlib.h>

class VE_EXPORT veMath
{
public:
	static const veReal PI;
	static const veReal TWO_PI;
	static const veReal HALF_PI;
	static const veReal QUARTER_PI;
	static const veReal FLOAT_MAX;
	static const veReal TOLERANCE;

	veMath();
	~veMath();

	
	static veReal veSqrt(veReal val) { return sqrt(val); }
	static veReal veSqr(veReal val) { return val * val; }
	static veReal veSin(veReal val) { return sin(val); }
	static veReal veCos(veReal val) { return cos(val); }
	static veReal veTan(veReal val) { return tan(val); }
	static veReal veAsin(veReal val) { return asin(val); }
	static veReal veAcos(veReal val) { return acos(val); }
	static veReal veAtan(veReal val) { return atan(val); }
	static veReal veAtan2(veReal y, veReal x) { return atan2(y, x); }
	static veReal veAbs(veReal val) { return fabs(val); }
	static veReal veRadian(veReal deg) { return deg * PI / 180.0f; }
	static veReal veDegree(veReal rad) { return rad * 180.0f / PI; }
	static veReal veRandomUnitization() {
#if (VE_PLATFORM == VE_PLATFORM_ANDROID)
		return veReal(drand48());
#else
		return (veReal)rand() / (veReal)RAND_MAX;
#endif
	}
	static bool isNaN(veReal val) { return val != val; }
	template<typename T> static T minimum(const T& lVal, const T& rVal) { return lVal < rVal ? lVal : rVal; }
	template<typename T> static T maximum(const T& lVal, const T& rVal) { return lVal < rVal ? rVal : lVal; }
};

#endif
