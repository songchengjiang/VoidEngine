#ifndef _VE_MATH_
#define _VE_MATH_
#include "Prerequisites.h"
#include <math.h>

class VE_EXPORT veMath
{
public:
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
	static veReal abs(veReal val) { return ::abs(val); }
	static bool isNaN(veReal val) { return val != val; }
};

#endif
