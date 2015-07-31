#include "Vector2.h"

const veVec2 veVec2::ZERO            = veVec2();
const veVec2 veVec2::UNIT_X          = veVec2(1.0f, 0.0f);
const veVec2 veVec2::UNIT_Y          = veVec2(0.0f, 1.0f);
const veVec2 veVec2::NEGATIVE_UNIT_X = veVec2(-1.0f, 0.0f);
const veVec2 veVec2::NEGATIVE_UNIT_Y = veVec2(0.0f, -1.0f);
const veVec2 veVec2::UNIT_SCALE      = veVec2(1.0f);

veVec2::veVec2()
	: _x(0.0f)
	, _y(0.0f)
{

}

veVec2::veVec2(const veVec2 &copy)
	: _x(copy._x)
	, _y(copy._y)
{

}

veVec2::veVec2(const veReal scaler)
	: _x(scaler)
	, _y(scaler)
{

}

veVec2::veVec2(veReal *xy)
	: _x(xy[0])
	, _y(xy[1])
{

}

veVec2::veVec2(const veReal x, const veReal y)
	: _x(x)
	, _y(y)
{

}

veVec2::~veVec2()
{

}
