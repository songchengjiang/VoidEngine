#ifndef _VE_VECTOR4_
#define _VE_VECTOR4_
#include "Prerequisites.h"
#include "BaseCore/Math.h"

class VE_EXPORT veVec4
{
public:

	// special points
	static const veVec4 BLACK;
	static const veVec4 WHITE;

	veVec4();
	veVec4(const veVec4 &copy);
	veVec4(const veReal scaler);
	veVec4(veReal *xyzw);
	veVec4(const veReal x, const veReal y, const veReal z, const veReal w);
	~veVec4();

	veReal& x() { return _x; };
	const veReal& x() const { return _x; };
	veReal& y() { return _y; };
	const veReal& y() const { return _y; };
	veReal& z() { return _z; };
	const veReal& z() const { return _z; };
	veReal& w() { return _w; };
	const veReal& w() const { return _w; };

	veReal& r() { return _x; };
	const veReal& r() const { return _x; };
	veReal& g() { return _y; };
	const veReal& g() const { return _y; };
	veReal& b() { return _z; };
	const veReal& b() const { return _z; };
	veReal& a() { return _w; };
	const veReal& a() const { return _w; };

	bool isNaN() const{
		return veMath::isNaN(_x) || veMath::isNaN(_y) || veMath::isNaN(_z);
	}

	bool isZeroLength(void) const{
		veReal sqlen = (_x * _x) + (_y * _y) + (_z * _z) + (_w * _w);
		return (sqlen < (1e-06 * 1e-06));
	}

	inline veReal operator [] (const size_t i) const{
		veAssert(i < 4);
		return *(&_x + i);
	}

	inline veReal& operator [] (const size_t i){
		veAssert(i < 4);

		return *(&_x + i);
	}

	inline veVec4& operator = (const veVec4& rkVector){
		_x = rkVector._x;
		_y = rkVector._y;
		_z = rkVector._z;
		_w = rkVector._w;

		return *this;
	}

	inline veVec4& operator = (const veReal fScalar){
		_x = fScalar;
		_y = fScalar;
		_z = fScalar;
		_w = fScalar;

		return *this;
	}

	inline bool operator == (const veVec4& rkVector) const{
		return (_x == rkVector._x && _y == rkVector._y && _z == rkVector._z && _w == rkVector._w);
	}

	inline bool operator != (const veVec4& rkVector) const{
		return (_x != rkVector._x || _y != rkVector._y || _z != rkVector._z || _w != rkVector._w);
	}

	// arithmetic operations
	inline veVec4 operator + (const veVec4& rkVector) const{
		return veVec4(_x + rkVector._x, _y + rkVector._y, _z + rkVector._z, _w + rkVector._w);
	}

	inline veVec4 operator - (const veVec4& rkVector) const{
		return veVec4(_x - rkVector._x, _y - rkVector._y, _z - rkVector._z, _w - rkVector._w);
	}

	inline veVec4 operator * (const veReal fScalar) const{
		return veVec4(_x * fScalar, _y * fScalar, _z * fScalar, _w * fScalar);
	}

	inline veVec4 operator * (const veVec4& rhs) const{
		return veVec4(_x * rhs._x, _y * rhs._y, _z * rhs._z, _w * rhs._w);
	}

	inline veVec4 operator / (const veReal fScalar) const{
		veAssert(fScalar != 0.0);

		veReal fInv = 1.0f / fScalar;

		return veVec4(_x * fInv, _y * fInv, _z * fInv, _w * fInv);
	}

	inline veVec4 operator / (const veVec4& rhs) const{
		return veVec4(_x / rhs._x, _y / rhs._y, _z / rhs._z, _w / rhs._w);
	}

	inline const veVec4& operator + () const{
		return *this;
	}

	inline veVec4 operator - () const{
		return veVec4(-_x, -_y, -_z, -_w);
	}

	// arithmetic updates
	inline veVec4& operator += (const veVec4& rkVector){
		_x += rkVector._x;
		_y += rkVector._y;
		_z += rkVector._z;
		_w += rkVector._w;

		return *this;
	}

	inline veVec4& operator += (const veReal fScaler){
		_x += fScaler;
		_y += fScaler;
		_z += fScaler;
		_w += fScaler;

		return *this;
	}

	inline veVec4& operator -= (const veVec4& rkVector){
		_x -= rkVector._x;
		_y -= rkVector._y;
		_z -= rkVector._z;
		_w -= rkVector._w;

		return *this;
	}

	inline veVec4& operator -= (const veReal fScaler){
		_x -= fScaler;
		_y -= fScaler;
		_z -= fScaler;
		_w -= fScaler;

		return *this;
	}

	inline veVec4& operator *= (const veReal fScalar){
		_x *= fScalar;
		_y *= fScalar;
		_z *= fScalar;
		_w *= fScalar;

		return *this;
	}

	inline veVec4& operator *= (const veVec4& rkVector){
		_x *= rkVector._x;
		_y *= rkVector._y;
		_z *= rkVector._z;
		_w *= rkVector._z;

		return *this;
	}

	inline veVec4& operator /= (const veReal fScalar){
		veAssert(fScalar != 0.0);

		veReal fInv = 1.0f / fScalar;

		_x *= fInv;
		_y *= fInv;
		_z *= fInv;
		_w *= fInv;

		return *this;
	}

	inline veVec4& operator /= (const veVec4& rkVector){
		_x /= rkVector._x;
		_y /= rkVector._y;
		_z /= rkVector._z;
		_w /= rkVector._w;

		return *this;
	}

	inline bool operator < (const veVec4& rhs) const{
		if (_x < rhs._x && _y < rhs._y  && _z < rhs._z  && _w < rhs._w)
			return true;
		return false;
	}

	inline bool operator >(const veVec4& rhs) const{
		if (rhs._x < _x && rhs._y < _y && rhs._z < _z && rhs._w < _w)
			return true;
		return false;
	}

	friend veVec4 operator * (const veReal fScalar, const veVec4& rkVector);
	friend veVec4 operator / (const veReal fScalar, const veVec4& rkVector);
	friend veVec4 operator + (const veVec4& lhs, const veReal rhs);
	friend veVec4 operator + (const veReal lhs, const veVec4& rhs);
	friend veVec4 operator - (const veVec4& lhs, const veReal rhs);
	friend veVec4 operator - (const veReal lhs, const veVec4& rhs);

private:

	veReal _x;
	veReal _y;
	veReal _z;
	veReal _w;
};

static veVec4 operator * (const veReal fScalar, const veVec4& rkVector){
	return veVec4(fScalar * rkVector._x, fScalar * rkVector._y, fScalar * rkVector._z, fScalar * rkVector._w);
}

static veVec4 operator / (const veReal fScalar, const veVec4& rkVector){
	return veVec4(fScalar / rkVector._x, fScalar / rkVector._y, fScalar / rkVector._z, fScalar / rkVector._w);
}

static veVec4 operator + (const veVec4& lhs, const veReal rhs){
	return veVec4(lhs._x + rhs, lhs._y + rhs, lhs._z + rhs, lhs._w + rhs);
}

static veVec4 operator + (const veReal lhs, const veVec4& rhs){
	return veVec4(lhs + rhs._x, lhs + rhs._y, lhs + rhs._z, lhs + rhs._w);
}

static veVec4 operator - (const veVec4& lhs, const veReal rhs){
	return veVec4(lhs._x - rhs, lhs._y - rhs, lhs._z - rhs, lhs._w - rhs);
}

static veVec4 operator - (const veReal lhs, const veVec4& rhs){
	return veVec4(lhs - rhs._x, lhs - rhs._y, lhs - rhs._z, lhs - rhs._w);
}

typedef std::vector<veVec4> veVec4Array;

#endif