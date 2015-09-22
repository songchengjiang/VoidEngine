#ifndef _VE_VECTOR2_
#define _VE_VECTOR2_
#include "Prerequisites.h"
#include "BaseCore/Math.h"

class VE_EXPORT veVec2
{
public:

	// special points
	static const veVec2 ZERO;
	static const veVec2 UNIT_X;
	static const veVec2 UNIT_Y;
	static const veVec2 NEGATIVE_UNIT_X;
	static const veVec2 NEGATIVE_UNIT_Y;
	static const veVec2 UNIT_SCALE;

	veVec2();
	veVec2(const veVec2 &copy);
	veVec2(const veReal scaler);
	veVec2(veReal *xy);
	veVec2(const veReal x, const veReal y);
	~veVec2();

	veReal& x() { return _x; };
	const veReal& x() const { return _x; };
	veReal& y() { return _y; };
	const veReal& y() const { return _y; };

	veReal length() const { return veMath::sqrt(_x * _x + _y * _y); }
	veReal squaredLength() const { return _x * _x + _y * _y; }
	veReal distance(const veVec2& rhs) const { return (*this - rhs).length(); }
	veReal squaredDistance(const veVec2& rhs) const{ return (*this - rhs).squaredLength(); }
	veReal dotProduct(const veVec2 &vec) const { return _x * vec._x + _y * vec._y;  }
	veReal crossProduct(const veVec2 &vec) const { return _x * vec._y - _y * vec._x; }

	veReal normalize(){
		veReal fLength = veMath::sqrt(_x * _x + _y * _y);

		if (fLength > veReal(0.0f)){
			veReal fInvLength = 1.0f / fLength;
			_x *= fInvLength;
			_y *= fInvLength;
		}

		return fLength;
	}

	veVec2 reflect(const veVec2& normal) const{
		return veVec2(*this - (2 * this->dotProduct(normal) * normal));
	}

	bool isNaN() const{
		return veMath::isNaN(_x) || veMath::isNaN(_y);
	}

	bool isZeroLength(void) const{
		veReal sqlen = (_x * _x) + (_y * _y);
		return (sqlen < (1e-06 * 1e-06));
	}

	inline veReal operator [] (const size_t i) const{
		veAssert(i < 2);
		return *(&_x + i);
	}

	inline veReal& operator [] (const size_t i){
		veAssert(i < 2);

		return *(&_x + i);
	}

	inline veVec2& operator = (const veVec2& rkVector){
		_x = rkVector._x;
		_y = rkVector._y;

		return *this;
	}

	inline veVec2& operator = (const veReal fScalar){
		_x = fScalar;
		_y = fScalar;

		return *this;
	}

	inline bool operator == (const veVec2& rkVector) const{
		return (_x == rkVector._x && _y == rkVector._y);
	}

	inline bool operator != (const veVec2& rkVector) const{
		return (_x != rkVector._x || _y != rkVector._y);
	}

	// arithmetic operations
	inline veVec2 operator + (const veVec2& rkVector) const{
		return veVec2(_x + rkVector._x, _y + rkVector._y);
	}

	inline veVec2 operator - (const veVec2& rkVector) const{
		return veVec2(_x - rkVector._x, _y - rkVector._y);
	}

	inline veVec2 operator * (const veReal fScalar) const{
		return veVec2(_x * fScalar, _y * fScalar);
	}

	inline veVec2 operator * (const veVec2& rhs) const{
		return veVec2(_x * rhs._x, _y * rhs._y);
	}

	inline veVec2 operator / (const veReal fScalar) const{
		veAssert(fScalar != 0.0);

		veReal fInv = 1.0f / fScalar;

		return veVec2(_x * fInv, _y * fInv);
	}

	inline veVec2 operator / (const veVec2& rhs) const{
		return veVec2(_x / rhs._x, _y / rhs._y);
	}

	inline const veVec2& operator + () const{
		return *this;
	}

	inline veVec2 operator - () const{
		return veVec2(-_x, -_y);
	}

	// arithmetic updates
	inline veVec2& operator += (const veVec2& rkVector){
		_x += rkVector._x;
		_y += rkVector._y;

		return *this;
	}

	inline veVec2& operator += (const veReal fScaler){
		_x += fScaler;
		_y += fScaler;

		return *this;
	}

	inline veVec2& operator -= (const veVec2& rkVector){
		_x -= rkVector._x;
		_y -= rkVector._y;

		return *this;
	}

	inline veVec2& operator -= (const veReal fScaler){
		_x -= fScaler;
		_y -= fScaler;

		return *this;
	}

	inline veVec2& operator *= (const veReal fScalar){
		_x *= fScalar;
		_y *= fScalar;

		return *this;
	}

	inline veVec2& operator *= (const veVec2& rkVector){
		_x *= rkVector._x;
		_y *= rkVector._y;

		return *this;
	}

	inline veVec2& operator /= (const veReal fScalar){
		veAssert(fScalar != 0.0);

		veReal fInv = 1.0f / fScalar;

		_x *= fInv;
		_y *= fInv;

		return *this;
	}

	inline veVec2& operator /= (const veVec2& rkVector){
		_x /= rkVector._x;
		_y /= rkVector._y;

		return *this;
	}

	inline bool operator < (const veVec2& rhs) const{
		if (_x < rhs._x && _y < rhs._y)
			return true;
		return false;
	}

	inline bool operator >(const veVec2& rhs) const{
		if (rhs._x < _x && rhs._y < _y)
			return true;
		return false;
	}

    friend veVec2 operator * (const veReal fScalar, const veVec2& rkVector){
        return veVec2(fScalar * rkVector._x, fScalar * rkVector._y);
    }
    friend veVec2 operator / (const veReal fScalar, const veVec2& rkVector){
        return veVec2(fScalar / rkVector._x, fScalar / rkVector._y);
    }
    friend veVec2 operator + (const veVec2& lhs, const veReal rhs){
        return veVec2(lhs._x + rhs, lhs._y + rhs);
    }
    friend veVec2 operator + (const veReal lhs, const veVec2& rhs){
        return veVec2(lhs + rhs._x, lhs + rhs._y);
    }
    friend veVec2 operator - (const veVec2& lhs, const veReal rhs){
        return veVec2(lhs._x - rhs, lhs._y - rhs);
    }
    friend veVec2 operator - (const veReal lhs, const veVec2& rhs){
        return veVec2(lhs - rhs._x, lhs - rhs._y);
    }

private:

	veReal _x;
	veReal _y;
};

/*veVec2 operator * (const veReal fScalar, const veVec2& rkVector){
	return veVec2(fScalar * rkVector._x, fScalar * rkVector._y);
}

veVec2 operator / (const veReal fScalar, const veVec2& rkVector){
	return veVec2(fScalar / rkVector._x, fScalar / rkVector._y);
}

veVec2 operator + (const veVec2& lhs, const veReal rhs){
	return veVec2(lhs._x + rhs, lhs._y + rhs);
}

veVec2 operator + (const veReal lhs, const veVec2& rhs){
	return veVec2(lhs + rhs._x, lhs + rhs._y);
}

veVec2 operator - (const veVec2& lhs, const veReal rhs){
	return veVec2(lhs._x - rhs, lhs._y - rhs);
}

veVec2 operator - (const veReal lhs, const veVec2& rhs){
	return veVec2(lhs - rhs._x, lhs - rhs._y);
}*/

typedef std::vector<veVec2> veVec2Array;

#endif