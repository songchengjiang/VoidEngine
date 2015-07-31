#ifndef _VE_QUATERNION_
#define _VE_QUATERNION_
#include "Prerequisites.h"
#include "BaseCore/Math.h"
#include "Vector3.h"

class veVec3;
class VE_EXPORT veQuat
{
public:

	static const veQuat ZERO;
	static const veQuat IDENTITY;

	veQuat();
	veQuat(const veReal w, const veReal x, const veReal y, const veReal z);
	veQuat(const veReal angle, const veVec3 &axes);
	~veQuat();

	veReal& w() { return _w; };
	const veReal& w() const { return _w; };
	veReal& x() { return _x; };
	const veReal& x() const { return _x; };
	veReal& y() { return _y; };
	const veReal& y() const { return _y; };
	veReal& z() { return _z; };
	const veReal& z() const { return _z; };

	void set(const veReal angle, const veVec3 &axes);
	veReal dot(const veQuat &rkQ) const;
	veReal normalize();
	void inverse();
	void unitInverse();
	veReal roll(bool reprojectAxis = true) const;
	veReal pitch(bool reprojectAxis = true) const;
	veReal yaw(bool reprojectAxis = true) const;
	veReal normLength() const { return _w * _w + _x * _x + _y * _y + _z * _z; }

	bool isNaN() const
	{
		return veMath::isNaN(_x) || veMath::isNaN(_y) || veMath::isNaN(_z) || veMath::isNaN(_w);
	}

	static veQuat slerp(veReal fT, const veQuat& rkP, const veQuat& rkQ, bool shortestPath = false);
	//nlerp is faster than Slerp.
	static veQuat nlerp(veReal fT, const veQuat& rkP, const veQuat& rkQ, bool shortestPath = false);
	static veQuat squad(veReal fT, const veQuat& rkP,
		const veQuat& rkA, const veQuat& rkB,
		const veQuat& rkQ, bool shortestPath = false);

	inline veReal operator [] (const size_t i) const{
		veAssert(i < 4);

		return *(&_w + i);
	}

	/// Array accessor operator
	inline veReal& operator [] (const size_t i){
		veAssert(i < 4);

		return *(&_w + i);
	}

	inline veQuat& operator= (const veQuat& rkQ){
		_w = rkQ._w;
		_x = rkQ._x;
		_y = rkQ._y;
		_z = rkQ._z;
		return *this;
	}

	veQuat operator+ (const veQuat& rkQ) const{
		return veQuat(_w + rkQ._w, _x + rkQ._x, _y + rkQ._y, _z + rkQ._z);
	}

	veQuat operator- (const veQuat& rkQ) const{
		return veQuat(_w - rkQ._w, _x - rkQ._x, _y - rkQ._y, _z - rkQ._z);
	}

	veQuat operator* (const veQuat& rkQ) const{
		// NOTE:  Multiplication is not generally commutative, so in most
		// cases p*q != q*p.
		return veQuat
			(
			_w * rkQ._w - _x * rkQ._x - _y * rkQ._y - _z * rkQ._z,
			_w * rkQ._x + _x * rkQ._w + _y * rkQ._z - _z * rkQ._y,
			_w * rkQ._y + _y * rkQ._w + _z * rkQ._x - _x * rkQ._z,
			_w * rkQ._z + _z * rkQ._w + _x * rkQ._y - _y * rkQ._x
			);
	}

	veQuat operator* (veReal fScalar) const{
		return veQuat(fScalar * _w, fScalar * _x, fScalar * _y, fScalar * _z);
	}

	veVec3 operator* (const veVec3& v) const{
		// nVidia SDK implementation
		veVec3 uv, uuv;
		veVec3 qvec(_x, _y, _z);
		uv = qvec.crossProduct(v);
		uuv = qvec.crossProduct(uv);
		uv *= (2.0f * _w);
		uuv *= 2.0f;

		return v + uv + uuv;
	}

	veQuat operator- () const{
		return veQuat(-_w, -_x, -_y, -_z);
	}

	inline bool operator== (const veQuat& rhs) const{
		return (rhs._x == _x) && (rhs._y == _y) &&
			(rhs._z == _z) && (rhs._w == _w);
	}

	inline bool operator!= (const veQuat& rhs) const{
		return !operator==(rhs);
	}

	friend veQuat operator * (const veReal fScalar, const veQuat& rkQ);
	friend veVec3 operator * (const veVec3& v, const veQuat& rkQ);

private:

	void fromAngleAxis(const veReal angle, const veVec3 &axes);

private:

	veReal _w;
	veReal _x;
	veReal _y;
	veReal _z;
};

static veQuat operator * (const veReal fScalar, const veQuat& rkQ){
	return veQuat(fScalar * rkQ._w, fScalar * rkQ._y, fScalar * rkQ._z, fScalar * rkQ._z);
}

static veVec3 operator * (const veVec3& v, const veQuat& rkQ){
	// nVidia SDK implementation
	veVec3 uv, uuv;
	veVec3 qvec(rkQ._x, rkQ._y, rkQ._z);
	uv = qvec.crossProduct(v);
	uuv = qvec.crossProduct(uv);
	uv *= (2.0f * rkQ._w);
	uuv *= 2.0f;

	return v + uv + uuv;
}

#endif
