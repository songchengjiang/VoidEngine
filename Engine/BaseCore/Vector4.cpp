#include "Vector4.h"

const veVec4 veVec4::BLACK = veVec4(0.0f, 0.0f, 0.0f, 1.0f);
const veVec4 veVec4::WHITE = veVec4(1.0f);

veVec4::veVec4()
	: _x(0.0f)
	, _y(0.0f)
	, _z(0.0f)
	, _w(0.0f)
{

}

veVec4::veVec4(const veVec4 &copy)
	: _x(copy._x)
	, _y(copy._y)
	, _z(copy._z)
	, _w(copy._w)
{

}

veVec4::veVec4(const veReal scaler)
	: _x(scaler)
	, _y(scaler)
	, _z(scaler)
	, _w(scaler)
{

}

veVec4::veVec4(veReal *xyzw)
	: _x(xyzw[0])
	, _y(xyzw[1])
	, _z(xyzw[2])
	, _w(xyzw[3])
{

}

veVec4::veVec4(const veReal x, const veReal y, const veReal z, const veReal w)
	: _x(x)
	, _y(y)
	, _z(z)
	, _w(w)
{

}

veVec4::~veVec4()
{

}
