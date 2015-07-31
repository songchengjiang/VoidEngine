#include "Quaternion.h"

const veQuat veQuat::ZERO = veQuat(0.0f, 0.0f, 0.0f, 0.0f);
const veQuat veQuat::IDENTITY = veQuat();
static const veReal msEpsilon = 1e-03f;

veQuat::veQuat()
	: _w(1.0f)
	, _x(0.0f)
	, _y(0.0f)
	, _z(0.0f)
{

}

veQuat::veQuat(const veReal w, const veReal x, const veReal y, const veReal z)
	: _w(w)
	, _x(x)
	, _y(y)
	, _z(z)
{

}

veQuat::veQuat(const veReal angle, const veVec3 &axes)
{
	this->fromAngleAxis(angle, axes);
}

veQuat::veQuat(const veQuat &copy)
	: _w(copy._w)
	, _x(copy._x)
	, _y(copy._y)
	, _z(copy._z)
{

}

veQuat::~veQuat()
{

}

void veQuat::set(const veReal angle, const veVec3 &axes)
{
	this->fromAngleAxis(angle, axes);
}

veReal veQuat::dot(const veQuat &rkQ) const
{
	return _w * rkQ._w + _x * rkQ._x + _y * rkQ._y + _z * rkQ._z;
}

veReal veQuat::normalize()
{
	veReal len = normLength();
	veReal factor = 1.0f / veMath::sqrt(len);
	*this = *this * factor;
	return len;
}

void veQuat::inverse()
{
	veReal fNorm = normLength();
	if (fNorm > 0.0)
	{
		veReal fInvNorm = 1.0f / fNorm;
		_w *= fInvNorm;
		_x *= -fInvNorm;
		_y *= -fInvNorm;
		_z *= -fInvNorm;
	}
}

void veQuat::unitInverse()
{
	_x = -_x;
	_y = -_y;
	_z = -_z;
}

veReal veQuat::roll(bool reprojectAxis) const
{
	if (reprojectAxis)
	{
		// roll = atan2(localx.y, localx.x)
		// pick parts of xAxis() implementation that we need
		//			Real fTx  = 2.0*x;
		veReal fTy = 2.0f * _y;
		veReal fTz = 2.0f * _z;
		veReal fTwz = fTz * _w;
		veReal fTxy = fTy * _x;
		veReal fTyy = fTy * _y;
		veReal fTzz = fTz * _z;

		// Vector3(1.0-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);

		return veMath::atan2(fTxy + fTwz, 1.0f - (fTyy + fTzz));

	}
	else
	{
		return veMath::atan2(2 * (_x * _y + _w * _z), _w * _w + _x * _x - _y * _y - _z * _z);
	}
}

veReal veQuat::pitch(bool reprojectAxis) const
{
	if (reprojectAxis)
	{
		// pitch = atan2(localy.z, localy.y)
		// pick parts of yAxis() implementation that we need
		veReal fTx = 2.0f * _x;
		//			Real fTy  = 2.0f*y;
		veReal fTz = 2.0f * _z;
		veReal fTwx = fTx * _w;
		veReal fTxx = fTx * _x;
		veReal fTyz = fTz * _y;
		veReal fTzz = fTz * _z;

		// Vector3(fTxy-fTwz, 1.0-(fTxx+fTzz), fTyz+fTwx);
		return veMath::atan2(fTyz + fTwx, 1.0f - (fTxx + fTzz));
	}
	else
	{
		// internal version
		return veMath::atan2(2 * (_y * _z + _w * _x), _w * _w - _x * _x - _y * _y + _z * _z);
	}
}

veReal veQuat::yaw(bool reprojectAxis) const
{
	if (reprojectAxis)
	{
		// yaw = atan2(localz.x, localz.z)
		// pick parts of zAxis() implementation that we need
		veReal fTx = 2.0f * _x;
		veReal fTy = 2.0f * _y;
		veReal fTz = 2.0f * _z;
		veReal fTwy = fTy * _w;
		veReal fTxx = fTx * _x;
		veReal fTxz = fTz * _x;
		veReal fTyy = fTy * _y;

		// Vector3(fTxz+fTwy, fTyz-fTwx, 1.0-(fTxx+fTyy));

		return veMath::atan2(fTxz + fTwy, 1.0f - (fTxx + fTyy));

	}
	else
	{
		// internal version
		return veMath::asin(-2 * (_x * _z - _w * _y));
	}
}

veQuat veQuat::slerp(veReal fT, const veQuat& rkP, const veQuat& rkQ, bool shortestPath /*= false*/)
{
	veReal fCos = rkP.dot(rkQ);
	veQuat rkT;

	// Do we need to invert rotation?
	if (fCos < 0.0f && shortestPath)
	{
		fCos = -fCos;
		rkT = -rkQ;
	}
	else
	{
		rkT = rkQ;
	}

	if (veMath::abs(fCos) < 1 - msEpsilon)
	{
		// Standard case (slerp)
		veReal fSin = veMath::sqrt(1 - veMath::sqr(fCos));
		veReal fAngle = veMath::atan2(fSin, fCos);
		veReal fInvSin = 1.0f / fSin;
		veReal fCoeff0 = veMath::sin((1.0f - fT) * fAngle) * fInvSin;
		veReal fCoeff1 = veMath::sin(fT * fAngle) * fInvSin;
		return fCoeff0 * rkP + fCoeff1 * rkT;
	}
	else
	{
		// There are two situations:
		// 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
		//    interpolation safely.
		// 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
		//    are an infinite number of possibilities interpolation. but we haven't
		//    have method to fix this case, so just use linear interpolation here.
		veQuat t = (1.0f - fT) * rkP + fT * rkT;
		// taking the complement requires renormalisation
		t.normalize();
		return t;
	}
}

veQuat veQuat::nlerp(veReal fT, const veQuat& rkP, const veQuat& rkQ, bool shortestPath /*= false*/)
{
	veQuat result;
	veReal fCos = rkP.dot(rkQ);
	if (fCos < 0.0f && shortestPath)
	{
		result = rkP + fT * ((-rkQ) - rkP);
	}
	else
	{
		result = rkP + fT * (rkQ - rkP);
	}
	result.normalize();
	return result;
}

veQuat veQuat::squad(veReal fT, const veQuat& rkP, const veQuat& rkA, const veQuat& rkB, const veQuat& rkQ, bool shortestPath /*= false*/)
{
	veReal fSlerpT = 2.0f*fT*(1.0f - fT);
	veQuat kSlerpP = slerp(fT, rkP, rkQ, shortestPath);
	veQuat kSlerpQ = slerp(fT, rkA, rkB);
	return slerp(fSlerpT, kSlerpP, kSlerpQ);
}

void veQuat::fromAngleAxis(const veReal angle, const veVec3 &axes)
{
	veReal fHalfAngle(0.5f*angle);
	veReal fSin = veMath::sin(fHalfAngle);
	_w = veMath::cos(fHalfAngle);
	_x = fSin * axes.x();
	_y = fSin * axes.y();
	_z = fSin * axes.z();
}
