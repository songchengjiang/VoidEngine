#ifndef _VE_MATH_
#define _VE_MATH_
#include "Prerequisites.h"
#include <cmath>
#include <random>

class VE_EXPORT veMath
{
public:
	static const veReal PI;
	static const veReal TWO_PI;
	static const veReal HALF_PI;
	static const veReal QUARTER_PI;

	veMath();
	~veMath();

	static veReal veSqrt(veReal val) { return std::sqrt(val); }
	static veReal veSqr(veReal val) { return val * val; }
	static veReal veSin(veReal val) { return std::sin(val); }
	static veReal veCos(veReal val) { return std::cos(val); }
	static veReal veTan(veReal val) { return std::tan(val); }
	static veReal veAsin(veReal val) { return std::asin(val); }
	static veReal veAcos(veReal val) { return std::acos(val); }
	static veReal veAtan(veReal val) { return std::atan(val); }
	static veReal veAtan2(veReal y, veReal x) { return std::atan2(y, x); }
	static veReal veAbs(veReal val) { return std::fabs(val); }
	static veReal veRadian(veReal deg) { return deg * PI / 180.0f; }
	static veReal veDegree(veReal rad) { return rad * 180.0f / PI; }
	static veReal veRandomUnitization() { return (veReal)std::rand() / RAND_MAX; }
	static bool isNaN(veReal val) { return val != val; }
	template<typename T> static T minimum(const T& lVal, const T& rVal) { return lVal < rVal ? lVal : rVal; }
	template<typename T> static T maximum(const T& lVal, const T& rVal) { return lVal < rVal ? rVal : lVal; }
};

#endif
