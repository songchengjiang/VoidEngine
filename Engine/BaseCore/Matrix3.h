#ifndef _VE_MATRIX3_
#define _VE_MATRIX3_
#include "Prerequisites.h"
#include "Quaternion.h"
#include "Vector3.h"

/*
The generic form M * V which shows the layout of the matrix
entries is shown below:
<pre>
[ m[0][0]  m[0][1]  m[0][2]]   {x}
| m[1][0]  m[1][1]  m[1][2]| * {y}
[ m[2][0]  m[2][1]  m[2][2]]   {z}
</pre>
*/

class VE_EXPORT veMat3
{
public:
	static const veMat3 IDENTITY;

	veMat3();
	veMat3(const veMat3 &copy);
	veMat3(const veQuat &rot);
	inline veMat3(veReal m00, veReal m01, veReal m02,
		veReal m10, veReal m11, veReal m12,
		veReal m20, veReal m21, veReal m22);
	~veMat3();

	inline void identity();
	inline void set(veReal m00, veReal m01, veReal m02,
		veReal m10, veReal m11, veReal m12,
		veReal m20, veReal m21, veReal m22);
	inline void transpose();
	inline void inverse();

	inline veReal* operator [] (size_t iRow){
		veAssert(iRow < 3);
		return m[iRow];
	}

	inline const veReal *operator [] (size_t iRow) const{
		veAssert(iRow < 3);
		return m[iRow];
	}

	inline veMat3& operator= (const veMat3& rkMatrix)
	{
		memcpy(m, rkMatrix.m, 9 * sizeof(veReal));
		return *this;
	}

	inline bool operator== (const veMat3& rkMatrix) const{
		if (
			m[0][0] != rkMatrix.m[0][0] || m[0][1] != rkMatrix.m[0][1] || m[0][2] != rkMatrix.m[0][2] ||
			m[1][0] != rkMatrix.m[1][0] || m[1][1] != rkMatrix.m[1][1] || m[1][2] != rkMatrix.m[1][2] ||
			m[2][0] != rkMatrix.m[2][0] || m[2][1] != rkMatrix.m[2][1] || m[2][2] != rkMatrix.m[2][2])
			return false;
		return true;
	}

	inline bool operator!= (const veMat3& rkMatrix) const{
		return !operator==(rkMatrix);
	}

	inline veMat3 operator+ (const veMat3& rkMatrix) const{
		veMat3 r;

		r.m[0][0] = m[0][0] + rkMatrix.m[0][0];
		r.m[0][1] = m[0][1] + rkMatrix.m[0][1];
		r.m[0][2] = m[0][2] + rkMatrix.m[0][2];

		r.m[1][0] = m[1][0] + rkMatrix.m[1][0];
		r.m[1][1] = m[1][1] + rkMatrix.m[1][1];
		r.m[1][2] = m[1][2] + rkMatrix.m[1][2];

		r.m[2][0] = m[2][0] + rkMatrix.m[2][0];
		r.m[2][1] = m[2][1] + rkMatrix.m[2][1];
		r.m[2][2] = m[2][2] + rkMatrix.m[2][2];

		return r;
	}

	inline veMat3 operator- (const veMat3& rkMatrix) const{
		veMat3 r;
		r.m[0][0] = m[0][0] - rkMatrix.m[0][0];
		r.m[0][1] = m[0][1] - rkMatrix.m[0][1];
		r.m[0][2] = m[0][2] - rkMatrix.m[0][2];

		r.m[1][0] = m[1][0] - rkMatrix.m[1][0];
		r.m[1][1] = m[1][1] - rkMatrix.m[1][1];
		r.m[1][2] = m[1][2] - rkMatrix.m[1][2];

		r.m[2][0] = m[2][0] - rkMatrix.m[2][0];
		r.m[2][1] = m[2][1] - rkMatrix.m[2][1];
		r.m[2][2] = m[2][2] - rkMatrix.m[2][2];

		return r;
	}

	inline veMat3 operator* (const veMat3& m2) const{
		veMat3 r;
		r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0];
		r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1];
		r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2];

		r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0];
		r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1];
		r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2];

		r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0];
		r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1];
		r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2];

		return r;
	}

	inline veMat3 operator* (const veReal scalar) const{
		veMat3 r;
		r.m[0][0] = m[0][0] * scalar;
		r.m[0][1] = m[0][1] * scalar;
		r.m[0][2] = m[0][2] * scalar;

		r.m[1][0] = m[1][0] * scalar;
		r.m[1][1] = m[1][1] * scalar;
		r.m[1][2] = m[1][2] * scalar;

		r.m[2][0] = m[2][0] * scalar;
		r.m[2][1] = m[2][1] * scalar;
		r.m[2][2] = m[2][2] * scalar;
		return r;
	}

	inline veVec3 operator* (const veVec3& v) const{
		veVec3 r;
		r.x() = (m[0][0] * v.x() + m[0][1] * v.y() + m[0][2] * v.z());
		r.y() = (m[1][0] * v.x() + m[1][1] * v.y() + m[1][2] * v.z());
		r.z() = (m[2][0] * v.x() + m[2][1] * v.y() + m[2][2] * v.z());
		return r;
	}

	friend veMat3 operator* (const veReal scalar, const veMat3& m);

private:

	union {
		veReal m[3][3];
		veReal _m[9];
	};
};

static veMat3 operator* (const veReal scalar, const veMat3& m){
	veMat3 r;
	r.m[0][0] = m[0][0] * scalar;
	r.m[0][1] = m[0][1] * scalar;
	r.m[0][2] = m[0][2] * scalar;

	r.m[1][0] = m[1][0] * scalar;
	r.m[1][1] = m[1][1] * scalar;
	r.m[1][2] = m[1][2] * scalar;

	r.m[2][0] = m[2][0] * scalar;
	r.m[2][1] = m[2][1] * scalar;
	r.m[2][2] = m[2][2] * scalar;
	return r;
}

#endif