#ifndef _VE_MATRIX4_
#define _VE_MATRIX4_
#include "Prerequisites.h"
#include "Quaternion.h"
#include "Vector3.h"
#include "Vector4.h"

/*
	The generic form M * V which shows the layout of the matrix
	entries is shown below:
	<pre>
	[ m[0][0]  m[0][1]  m[0][2]  m[0][3] ]   {x}
	| m[1][0]  m[1][1]  m[1][2]  m[1][3] | * {y}
	| m[2][0]  m[2][1]  m[2][2]  m[2][3] |   {z}
	[ m[3][0]  m[3][1]  m[3][2]  m[3][3] ]   {1}
	</pre>
*/

class VE_EXPORT veMat4
{
public:

	static const veMat4 IDENTITY;
	static const veMat4 ZERO;

	veMat4();
	veMat4(const veMat4 &copy);
	veMat4(const veQuat &rot);
	veMat4(const veVec3 &trans);
    veMat4(veReal m00, veReal m01, veReal m02, veReal m03,
		veReal m10, veReal m11, veReal m12, veReal m13,
		veReal m20, veReal m21, veReal m22, veReal m23,
		veReal m30, veReal m31, veReal m32, veReal m33);
	~veMat4();

	inline void identity();
    void set(veReal m00, veReal m01, veReal m02, veReal m03,
		veReal m10, veReal m11, veReal m12, veReal m13,
		veReal m20, veReal m21, veReal m22, veReal m23,
		veReal m30, veReal m31, veReal m32, veReal m33);
    void makeTranslation(const veVec3 &trans);
	inline void makeRotation(const veQuat &rot);
	inline void makeScale(const veVec3 &scl);
    void makeTransform(const veVec3& position, const veVec3& scale, const veQuat& orientation);
    void makeLookAt(const veVec3 &eye, const veVec3 &center, const veVec3 &up);
    void decomposition(veVec3* position, veVec3* scale, veQuat* orientation) const;
    void transpose();
    void inverse();
    veReal determinant() const;

	static veMat4 translation(const veVec3 &trans);
	static veMat4 rotation(const veQuat &rot);
	static veMat4 scale(const veVec3 &scl);
	static veMat4 transform(const veVec3& position, const veVec3& scale, const veQuat& orientation);
	static veMat4 lookAt(const veVec3 &eye, const veVec3 &center, const veVec3 &up);

	inline veMat4 operator * (const veMat4 &m2) const{
		veMat4 r;
		r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
		r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
		r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
		r.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

		r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
		r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
		r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
		r.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

		r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
		r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
		r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
		r.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

		r.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
		r.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
		r.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
		r.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];

		return r;
	}

	inline veReal* operator [] (size_t iRow){
		veAssert(iRow < 4);
		return m[iRow];
	}

	inline const veReal *operator [] (size_t iRow) const{
		veAssert(iRow < 4);
		return m[iRow];
	}

	inline veMat4& operator = (const veMat4& rkMatrix){
		memcpy(m, rkMatrix.m, 16 * sizeof(veReal));
		return *this;
	}

	inline veVec3 operator * (const veVec3 &v) const{
		veVec3 r;
		veReal fInvW = 1.0f / (m[3][0] * v.x() + m[3][1] * v.y() + m[3][2] * v.z() + m[3][3]);
		r.x() = (m[0][0] * v.x() + m[0][1] * v.y() + m[0][2] * v.z() + m[0][3]) * fInvW;
		r.y() = (m[1][0] * v.x() + m[1][1] * v.y() + m[1][2] * v.z() + m[1][3]) * fInvW;
		r.z() = (m[2][0] * v.x() + m[2][1] * v.y() + m[2][2] * v.z() + m[2][3]) * fInvW;

		return r;
	}

	inline veVec4 operator * (const veVec4& v) const{
		return veVec4(
			m[0][0] * v.x() + m[0][1] * v.y() + m[0][2] * v.z() + m[0][3] * v.w(),
			m[1][0] * v.x() + m[1][1] * v.y() + m[1][2] * v.z() + m[1][3] * v.w(),
			m[2][0] * v.x() + m[2][1] * v.y() + m[2][2] * v.z() + m[2][3] * v.w(),
			m[3][0] * v.x() + m[3][1] * v.y() + m[3][2] * v.z() + m[3][3] * v.w()
			);
	}

	inline veMat4 operator * (const veReal& scaler) const {
		return veMat4(
			m[0][0] * scaler, m[0][1] * scaler, m[0][2] * scaler, m[0][3] * scaler,
			m[1][0] * scaler, m[1][1] * scaler, m[1][2] * scaler, m[1][3] * scaler,
			m[2][0] * scaler, m[2][1] * scaler, m[2][2] * scaler, m[2][3] * scaler,
			m[3][0] * scaler, m[3][1] * scaler, m[3][2] * scaler, m[3][3] * scaler
			);
	}

	inline veMat4 operator + (const veMat4 &m2) const{
		veMat4 r;

		r.m[0][0] = m[0][0] + m2.m[0][0];
		r.m[0][1] = m[0][1] + m2.m[0][1];
		r.m[0][2] = m[0][2] + m2.m[0][2];
		r.m[0][3] = m[0][3] + m2.m[0][3];

		r.m[1][0] = m[1][0] + m2.m[1][0];
		r.m[1][1] = m[1][1] + m2.m[1][1];
		r.m[1][2] = m[1][2] + m2.m[1][2];
		r.m[1][3] = m[1][3] + m2.m[1][3];

		r.m[2][0] = m[2][0] + m2.m[2][0];
		r.m[2][1] = m[2][1] + m2.m[2][1];
		r.m[2][2] = m[2][2] + m2.m[2][2];
		r.m[2][3] = m[2][3] + m2.m[2][3];

		r.m[3][0] = m[3][0] + m2.m[3][0];
		r.m[3][1] = m[3][1] + m2.m[3][1];
		r.m[3][2] = m[3][2] + m2.m[3][2];
		r.m[3][3] = m[3][3] + m2.m[3][3];

		return r;
	}

	inline veMat4 operator - (const veMat4 &m2) const{
		veMat4 r;
		r.m[0][0] = m[0][0] - m2.m[0][0];
		r.m[0][1] = m[0][1] - m2.m[0][1];
		r.m[0][2] = m[0][2] - m2.m[0][2];
		r.m[0][3] = m[0][3] - m2.m[0][3];

		r.m[1][0] = m[1][0] - m2.m[1][0];
		r.m[1][1] = m[1][1] - m2.m[1][1];
		r.m[1][2] = m[1][2] - m2.m[1][2];
		r.m[1][3] = m[1][3] - m2.m[1][3];

		r.m[2][0] = m[2][0] - m2.m[2][0];
		r.m[2][1] = m[2][1] - m2.m[2][1];
		r.m[2][2] = m[2][2] - m2.m[2][2];
		r.m[2][3] = m[2][3] - m2.m[2][3];

		r.m[3][0] = m[3][0] - m2.m[3][0];
		r.m[3][1] = m[3][1] - m2.m[3][1];
		r.m[3][2] = m[3][2] - m2.m[3][2];
		r.m[3][3] = m[3][3] - m2.m[3][3];

		return r;
	}

	inline bool operator == (const veMat4& m2) const{
		if (
			m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
			m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
			m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
			m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3])
			return false;
		return true;
	}

	inline bool operator != (const veMat4& m2) const{
		return !operator==(m2);
	}

private:

	union {
		veReal m[4][4];
		veReal _m[16];
	};
};

#endif