#ifndef _VE_VECTOR3_
#define _VE_VECTOR3_
#include "Prerequisites.h"
#include "BaseCore/MathAlgorithm.h"

class VE_EXPORT veVec3
{
public:

	// special points
	static const veVec3 ZERO;
	static const veVec3 UNIT_X;
	static const veVec3 UNIT_Y;
	static const veVec3 UNIT_Z;
	static const veVec3 NEGATIVE_UNIT_X;
	static const veVec3 NEGATIVE_UNIT_Y;
	static const veVec3 NEGATIVE_UNIT_Z;
	static const veVec3 UNIT_SCALE;

	veVec3();
	veVec3(const veVec3 &copy);
	veVec3(const veReal scaler);
	veVec3(veReal *xyz);
	veVec3(const veReal x, const veReal y, const veReal z);
	~veVec3();

	veReal& x() { return _x; };
	const veReal& x() const { return _x; };
	veReal& y() { return _y; };
	const veReal& y() const { return _y; };
	veReal& z() { return _z; };
	const veReal& z() const { return _z; };

	veReal length() const { return veMath::veSqrt(_x * _x + _y * _y + _z * _z); }
	veReal squaredLength() const { return _x * _x + _y * _y + _z * _z; }
	veReal distance(const veVec3& rhs) const { return (*this - rhs).length(); }
	veReal squaredDistance(const veVec3& rhs) const{ return (*this - rhs).squaredLength(); }
	veReal dotProduct(const veVec3 &vec) const { return _x * vec._x + _y * vec._y + _z * vec._z; }
	veVec3 crossProduct(const veVec3 &vec) const {
		return veVec3(_y * vec._z - _z * vec._y, _z * vec._x - _x * vec._z, _x * vec._y - _y * vec._x);
	}
    
    veVec3 perpendicular();
    veVec3 randomDeviat(veReal angle, const veVec3 &up = veVec3::ZERO);

	veReal normalize(){
		veReal fLength = veMath::veSqrt(_x * _x + _y * _y + _z * _z);

		if (fLength > veReal(0.0f))
		{
			veReal fInvLength = 1.0f / fLength;
			_x *= fInvLength;
			_y *= fInvLength;
			_z *= fInvLength;
		}

		return fLength;
	}

	veVec3 reflect(const veVec3& normal) const{
		return veVec3(*this - (2 * this->dotProduct(normal) * normal));
	}

	bool isNaN() const{
		return veMath::isNaN(_x) || veMath::isNaN(_y) || veMath::isNaN(_z);
	}

	bool isZeroLength(void) const{
		veReal sqlen = (_x * _x) + (_y * _y) + (_z * _z);
		return (sqlen < (1e-06 * 1e-06));
	}

	inline veReal operator [] (const size_t i) const{
		veAssert(i < 3);
		return *(&_x + i);
	}

	inline veReal& operator [] (const size_t i){
		veAssert(i < 3);

		return *(&_x + i);
	}

	inline veVec3& operator = (const veVec3& rkVector){
		_x = rkVector._x;
		_y = rkVector._y;
		_z = rkVector._z;

		return *this;
	}

	inline veVec3& operator = (const veReal fScalar){
		_x = fScalar;
		_y = fScalar;
		_z = fScalar;

		return *this;
	}

	inline bool operator == (const veVec3& rkVector) const{
		return (_x == rkVector._x && _y == rkVector._y && _z == rkVector._z);
	}

	inline bool operator != (const veVec3& rkVector) const{
		return (_x != rkVector._x || _y != rkVector._y || _z != rkVector._z);
	}

	// arithmetic operations
	inline veVec3 operator + (const veVec3& rkVector) const{
		return veVec3(_x + rkVector._x, _y + rkVector._y, _z + rkVector._z);
	}

	inline veVec3 operator - (const veVec3& rkVector) const{
		return veVec3(_x - rkVector._x, _y - rkVector._y, _z - rkVector._z);
	}

	inline veVec3 operator * (const veReal fScalar) const{
		return veVec3(_x * fScalar, _y * fScalar, _z * fScalar);
	}

	inline veVec3 operator * (const veVec3& rhs) const{
		return veVec3(_x * rhs._x, _y * rhs._y, _z * rhs._z);
	}

	inline veVec3 operator / (const veReal fScalar) const{
		veAssert(fScalar != 0.0);

		veReal fInv = 1.0f / fScalar;

		return veVec3(_x * fInv, _y * fInv, _z * fInv);
	}

	inline veVec3 operator / (const veVec3& rhs) const{
		return veVec3(_x / rhs._x, _y / rhs._y, _z / rhs._z);
	}

	inline const veVec3& operator + () const{
		return *this;
	}

	inline veVec3 operator - () const{
		return veVec3(-_x, -_y, -_z);
	}

	// arithmetic updates
	inline veVec3& operator += (const veVec3& rkVector){
		_x += rkVector._x;
		_y += rkVector._y;
		_z += rkVector._z;

		return *this;
	}

	inline veVec3& operator += (const veReal fScaler){
		_x += fScaler;
		_y += fScaler;
		_z += fScaler;

		return *this;
	}

	inline veVec3& operator -= (const veVec3& rkVector){
		_x -= rkVector._x;
		_y -= rkVector._y;
		_z -= rkVector._z;

		return *this;
	}

	inline veVec3& operator -= (const veReal fScaler){
		_x -= fScaler;
		_y -= fScaler;
		_z -= fScaler;

		return *this;
	}

	inline veVec3& operator *= (const veReal fScalar){
		_x *= fScalar;
		_y *= fScalar;
		_z *= fScalar;

		return *this;
	}

	inline veVec3& operator *= (const veVec3& rkVector){
		_x *= rkVector._x;
		_y *= rkVector._y;
		_z *= rkVector._z;

		return *this;
	}

	inline veVec3& operator /= (const veReal fScalar){
		veAssert(fScalar != 0.0);

		veReal fInv = 1.0f / fScalar;

		_x *= fInv;
		_y *= fInv;
		_z *= fInv;

		return *this;
	}

	inline veVec3& operator /= (const veVec3& rkVector){
		_x /= rkVector._x;
		_y /= rkVector._y;
		_z /= rkVector._z;

		return *this;
	}

	inline bool operator < (const veVec3& rhs) const{
		if (_x < rhs._x && _y < rhs._y  && _z < rhs._z)
			return true;
		return false;
	}

	inline bool operator >(const veVec3& rhs) const{
		if (rhs._x < _x && rhs._y < _y && rhs._z < _z)
			return true;
		return false;
	}

	inline bool operator <= (const veVec3& rhs) const {
		if (_x <= rhs._x && _y <= rhs._y  && _z <= rhs._z)
			return true;
		return false;
	}

	inline bool operator >=(const veVec3& rhs) const {
		if (rhs._x <= _x && rhs._y <= _y && rhs._z <= _z)
			return true;
		return false;
	}

    friend veVec3 operator * (const veReal fScalar, const veVec3& rkVector){
        return veVec3(fScalar * rkVector._x, fScalar * rkVector._y, fScalar * rkVector._z);
    }
    friend veVec3 operator / (const veReal fScalar, const veVec3& rkVector){
        return veVec3(fScalar / rkVector._x, fScalar / rkVector._y, fScalar / rkVector._z);
    }
    friend veVec3 operator + (const veVec3& lhs, const veReal rhs){
        return veVec3(lhs._x + rhs, lhs._y + rhs, lhs._z + rhs);
    }
    friend veVec3 operator + (const veReal lhs, const veVec3& rhs){
        return veVec3(lhs + rhs._x, lhs + rhs._y, lhs + rhs._z);
    }
    friend veVec3 operator - (const veVec3& lhs, const veReal rhs){
        return veVec3(lhs._x - rhs, lhs._y - rhs, lhs._z - rhs);
    }
    friend veVec3 operator - (const veReal lhs, const veVec3& rhs){
        return veVec3(lhs - rhs._x, lhs - rhs._y, lhs - rhs._z);
    }

private:

	veReal _x;
	veReal _y;
	veReal _z;
};

/*veVec3 operator * (const veReal fScalar, const veVec3& rkVector){
	return veVec3(fScalar * rkVector._x, fScalar * rkVector._y, fScalar * rkVector._z);
}

veVec3 operator / (const veReal fScalar, const veVec3& rkVector){
	return veVec3(fScalar / rkVector._x, fScalar / rkVector._y, fScalar / rkVector._z);
}

veVec3 operator + (const veVec3& lhs, const veReal rhs){
	return veVec3(lhs._x + rhs, lhs._y + rhs, lhs._z + rhs);
}

veVec3 operator + (const veReal lhs, const veVec3& rhs){
	return veVec3(lhs + rhs._x, lhs + rhs._y, lhs + rhs._z);
}

veVec3 operator - (const veVec3& lhs, const veReal rhs){
	return veVec3(lhs._x - rhs, lhs._y - rhs, lhs._z - rhs);
}

veVec3 operator - (const veReal lhs, const veVec3& rhs){
	return veVec3(lhs - rhs._x, lhs - rhs._y, lhs - rhs._z);
}*/

typedef std::vector<veVec3> veVec3Array;

#endif