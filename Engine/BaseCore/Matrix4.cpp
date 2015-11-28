#include "Matrix4.h"
#include "Matrix3.h"

static veReal MINOR(const veMat4& m, const size_t r0, const size_t r1, const size_t r2,
				const size_t c0, const size_t c1, const size_t c2)
{
	return m[r0][c0] * (m[r1][c1] * m[r2][c2] - m[r2][c1] * m[r1][c2]) -
		m[r0][c1] * (m[r1][c0] * m[r2][c2] - m[r2][c0] * m[r1][c2]) +
		m[r0][c2] * (m[r1][c0] * m[r2][c1] - m[r2][c0] * m[r1][c1]);
}

const veMat4 veMat4::IDENTITY = veMat4(1.0f, 0.0f, 0.0f, 0.0f
	                                 , 0.0f, 1.0f, 0.0f, 0.0f
									 , 0.0f, 0.0f, 1.0f, 0.0f
									 , 0.0f, 0.0f, 0.0f, 1.0f);

const veMat4 veMat4::ZERO = veMat4(0.0f, 0.0f, 0.0f, 0.0f
								 , 0.0f, 0.0f, 0.0f, 0.0f
								 , 0.0f, 0.0f, 0.0f, 0.0f
								 , 0.0f, 0.0f, 0.0f, 0.0f);

veMat4::veMat4()
{

}

veMat4::veMat4(const veMat4 &copy)
{
	memcpy(m, copy.m, 16 * sizeof(veReal));
}

veMat4::veMat4(veReal m00, veReal m01, veReal m02, veReal m03
	, veReal m10, veReal m11, veReal m12, veReal m13
	, veReal m20, veReal m21, veReal m22, veReal m23
	, veReal m30, veReal m31, veReal m32, veReal m33)
{
	set(m00, m01, m02, m03
		, m10, m11, m12, m13
		, m20, m21, m22, m23
		, m30, m31, m32, m33);
}

veMat4::veMat4(const veQuat &rot)
{
	makeRotation(rot);
}

veMat4::veMat4(const veVec3 &trans)
{
	makeTranslation(trans);
}

veMat4::~veMat4()
{

}

void veMat4::identity()
{
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;
	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[2][3] = 0.0f;
	m[3][0] = 0.0f;
	m[3][1] = 0.0f;
	m[3][2] = 0.0f;
	m[3][3] = 1.0f;
}

void veMat4::set(veReal m00, veReal m01, veReal m02, veReal m03
	, veReal m10, veReal m11, veReal m12, veReal m13
	, veReal m20, veReal m21, veReal m22, veReal m23
	, veReal m30, veReal m31, veReal m32, veReal m33)
{
	m[0][0] = m00;
	m[0][1] = m01;
	m[0][2] = m02;
	m[0][3] = m03;
	m[1][0] = m10;
	m[1][1] = m11;
	m[1][2] = m12;
	m[1][3] = m13;
	m[2][0] = m20;
	m[2][1] = m21;
	m[2][2] = m22;
	m[2][3] = m23;
	m[3][0] = m30;
	m[3][1] = m31;
	m[3][2] = m32;
	m[3][3] = m33;
}

void veMat4::makeRotation(const veQuat &rot)
{
	identity();
	veReal fTx = rot.x() + rot.x();
	veReal fTy = rot.y() + rot.y();
	veReal fTz = rot.z() + rot.z();
	veReal fTwx = fTx * rot.w();
	veReal fTwy = fTy * rot.w();
	veReal fTwz = fTz * rot.w();
	veReal fTxx = fTx * rot.x();
	veReal fTxy = fTy * rot.x();
	veReal fTxz = fTz * rot.x();
	veReal fTyy = fTy * rot.y();
	veReal fTyz = fTz * rot.y();
	veReal fTzz = fTz * rot.z();

	m[0][0] = 1.0f - (fTyy + fTzz);
	m[0][1] = fTxy - fTwz;
	m[0][2] = fTxz + fTwy;
	m[1][0] = fTxy + fTwz;
	m[1][1] = 1.0f - (fTxx + fTzz);
	m[1][2] = fTyz - fTwx;
	m[2][0] = fTxz - fTwy;
	m[2][1] = fTyz + fTwx;
	m[2][2] = 1.0f - (fTxx + fTyy);
}

void veMat4::makeTranslation(const veVec3 &trans)
{
	identity();
	m[0][3] = trans.x();
	m[1][3] = trans.y();
	m[2][3] = trans.z();
}

void veMat4::makeScale(const veVec3 &scl)
{
	identity();
	m[0][0] = scl.x();
	m[1][1] = scl.y();
	m[2][2] = scl.z();
}

void veMat4::makeTransform(const veVec3& position, const veVec3& scale, const veQuat& orientation)
{
	// Ordering:
	//    1. Scale
	//    2. Rotate
	//    3. Translate

	veReal fTx = orientation.x() + orientation.x();
	veReal fTy = orientation.y() + orientation.y();
	veReal fTz = orientation.z() + orientation.z();
	veReal fTwx = fTx * orientation.w();
	veReal fTwy = fTy * orientation.w();
	veReal fTwz = fTz * orientation.w();
	veReal fTxx = fTx * orientation.x();
	veReal fTxy = fTy * orientation.x();
	veReal fTxz = fTz * orientation.x();
	veReal fTyy = fTy * orientation.y();
	veReal fTyz = fTz * orientation.y();
	veReal fTzz = fTz * orientation.z();

	// Set up final matrix with scale, rotation and translation
	m[0][0] = scale.x() * (1.0f - (fTyy + fTzz)); m[0][1] = scale.y() * (fTxy - fTwz); m[0][2] = scale.z() * (fTxz + fTwy); m[0][3] = position.x();
	m[1][0] = scale.x() * (fTxy + fTwz); m[1][1] = scale.y() * (1.0f - (fTxx + fTzz)); m[1][2] = scale.z() * (fTyz - fTwx); m[1][3] = position.y();
	m[2][0] = scale.x() * (fTxz - fTwy); m[2][1] = scale.y() * (fTyz + fTwx); m[2][2] = scale.z() * (1.0f - (fTxx + fTyy)); m[2][3] = position.z();

	// No projection term
	m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
}

void veMat4::makeLookAt(const veVec3 &eye, const veVec3 &center, const veVec3 &up)
{
	veVec3 f = eye - center;
	f.normalize();
	veVec3 s = up.crossProduct(f);
	s.normalize();
	veVec3 u = f.crossProduct(s);
	if (f.isZeroLength() || s.isZeroLength() || u.isZeroLength()) return;

	float sdote = s.dotProduct(eye);
	float udote = u.dotProduct(eye);
	float fdote = f.dotProduct(eye);
	set(s.x(), u.x(), f.x(), s.x() * sdote + u.x() * udote + f.x() * fdote
		, s.y(), u.y(), f.y(), s.y() * sdote + u.y() * udote + f.y() * fdote
		, s.z(), u.z(), f.z(), s.z() * sdote + u.z() * udote + f.z() * fdote
		, 0.0f, 0.0f, 0.0f, 1.0f);
}

void veMat4::decomposition(veVec3* position, veVec3* scale, veQuat* orientation) const
{
	if (position){
		position->x() = m[0][3];
		position->y() = m[1][3];
		position->z() = m[2][3];
	}

	if (scale || orientation){
		veVec3 row0 = veVec3(m[0][0], m[0][1], m[0][2]);
		veVec3 row1 = veVec3(m[1][0], m[1][1], m[1][2]);
		veVec3 row2 = veVec3(m[2][0], m[2][1], m[2][2]);
		float row0Length = row0.length();
		float row1Length = row1.length();
		float row2Length = row2.length();
		if (scale) {
			scale->x() = row0Length;
			scale->y() = row1Length;
			scale->z() = row2Length;
		}

		if (orientation) {
			if (row0Length) {
				row0 /= row0Length;
			}

			if (row1Length) {
				row1 /= row1Length;
			}

			if (row2Length) {
				row2 /= row2Length;
			}

			orientation->set(veMat3(row0.x(), row0.y(), row0.z()
				                 ,  row1.x(), row1.y(), row1.z()
				                 ,  row2.x(), row2.y(), row2.z()));
		}
	}
}

void veMat4::transpose()
{
	set(m[0][0], m[1][0], m[2][0], m[3][0],
		m[0][1], m[1][1], m[2][1], m[3][1],
		m[0][2], m[1][2], m[2][2], m[3][2],
		m[0][3], m[1][3], m[2][3], m[3][3]);
}

void veMat4::inverse()
{
	veReal m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
	veReal m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
	veReal m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
	veReal m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];

	veReal v0 = m20 * m31 - m21 * m30;
	veReal v1 = m20 * m32 - m22 * m30;
	veReal v2 = m20 * m33 - m23 * m30;
	veReal v3 = m21 * m32 - m22 * m31;
	veReal v4 = m21 * m33 - m23 * m31;
	veReal v5 = m22 * m33 - m23 * m32;

	veReal t00 = +(v5 * m11 - v4 * m12 + v3 * m13);
	veReal t10 = -(v5 * m10 - v2 * m12 + v1 * m13);
	veReal t20 = +(v4 * m10 - v2 * m11 + v0 * m13);
	veReal t30 = -(v3 * m10 - v1 * m11 + v0 * m12);

	veReal invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

	veReal d00 = t00 * invDet;
	veReal d10 = t10 * invDet;
	veReal d20 = t20 * invDet;
	veReal d30 = t30 * invDet;

	veReal d01 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	veReal d11 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	veReal d21 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	veReal d31 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	v0 = m10 * m31 - m11 * m30;
	v1 = m10 * m32 - m12 * m30;
	v2 = m10 * m33 - m13 * m30;
	v3 = m11 * m32 - m12 * m31;
	v4 = m11 * m33 - m13 * m31;
	v5 = m12 * m33 - m13 * m32;

	veReal d02 = +(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	veReal d12 = -(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	veReal d22 = +(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	veReal d32 = -(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	v0 = m21 * m10 - m20 * m11;
	v1 = m22 * m10 - m20 * m12;
	v2 = m23 * m10 - m20 * m13;
	v3 = m22 * m11 - m21 * m12;
	v4 = m23 * m11 - m21 * m13;
	v5 = m23 * m12 - m22 * m13;

	veReal d03 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
	veReal d13 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
	veReal d23 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
	veReal d33 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

	set(d00, d01, d02, d03,
		d10, d11, d12, d13,
		d20, d21, d22, d23,
		d30, d31, d32, d33);
}

veReal veMat4::determinant() const
{
	return m[0][0] * MINOR(*this, 1, 2, 3, 1, 2, 3) -
		m[0][1] * MINOR(*this, 1, 2, 3, 0, 2, 3) +
		m[0][2] * MINOR(*this, 1, 2, 3, 0, 1, 3) -
		m[0][3] * MINOR(*this, 1, 2, 3, 0, 1, 2);
}

veMat4 veMat4::translation(const veVec3 &trans)
{
	veMat4 m;
	m.makeTranslation(trans);
	return m;
}

veMat4 veMat4::rotation(const veQuat &rot)
{
	veMat4 m;
	m.makeRotation(rot);
	return m;
}

veMat4 veMat4::scale(const veVec3 &scl)
{
	veMat4 m;
	m.makeScale(scl);
	return m;
}

veMat4 veMat4::transform(const veVec3& position, const veVec3& scale, const veQuat& orientation)
{
	veMat4 m;
	m.makeTransform(position, scale, orientation);
	return m;
}

veMat4 veMat4::lookAt(const veVec3 &eye, const veVec3 &center, const veVec3 &up)
{
	veMat4 m;
	m.makeLookAt(eye, center, up);
	return m;
}
