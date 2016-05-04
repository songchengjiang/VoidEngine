#include "Vector3.h"
#include "Quaternion.h"
#include "Matrix4.h"

const veVec3 veVec3::ZERO            = veVec3();
const veVec3 veVec3::UNIT_X          = veVec3(1.0f, 0.0f, 0.0f);
const veVec3 veVec3::UNIT_Y          = veVec3(0.0f, 1.0f, 0.0f);
const veVec3 veVec3::UNIT_Z          = veVec3(0.0f, 0.0f, 1.0f);
const veVec3 veVec3::NEGATIVE_UNIT_X = veVec3(-1.0f, 0.0f, 0.0f);
const veVec3 veVec3::NEGATIVE_UNIT_Y = veVec3(0.0f, -1.0f, 0.0f);
const veVec3 veVec3::NEGATIVE_UNIT_Z = veVec3(0.0f, 0.0f, -1.0f);
const veVec3 veVec3::UNIT_SCALE      = veVec3(1.0f);

veVec3::veVec3()
	: _x(0.0f)
	, _y(0.0f)
	, _z(0.0f)
{

}

veVec3::veVec3(const veVec3 &copy)
	: _x(copy._x)
	, _y(copy._y)
	, _z(copy._z)
{

}

veVec3::veVec3(const veReal scaler)
	: _x(scaler)
	, _y(scaler)
	, _z(scaler)
{

}

veVec3::veVec3(veReal *xyz)
	: _x(xyz[0])
	, _y(xyz[1])
	, _z(xyz[2])
{

}

veVec3::veVec3(const veReal x, const veReal y, const veReal z)
	: _x(x)
	, _y(y)
	, _z(z)
{

}

veVec3::~veVec3()
{

}

veVec3 veVec3::perpendicular()
{
    static const veReal fSquareZero = veReal(1e-06 * 1e-06);
    veVec3 perp;
    perp = this->crossProduct(veVec3::UNIT_X);
    if (perp.squaredLength() < fSquareZero){
        perp = this->crossProduct(veVec3::UNIT_Y);
    }
    perp.normalize();
    return perp;
}

veVec3 veVec3::randomDeviat(veReal angle, const veVec3 &up)
{
    veVec3 newUp = up == veVec3::ZERO? this->perpendicular() : up;
    veMat4 mat = veMat4::rotation(veQuat(veMath::veRandomUnitization() * veMath::TWO_PI, *this));
    newUp = mat * newUp;
    mat = veMat4::rotation(veQuat(angle, newUp));
    return mat * (*this);
}
