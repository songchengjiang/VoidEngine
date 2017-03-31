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

void veMat4::makeOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
{
    set(2.0f / (right - left), 0.0f                 , 0.0f                 , -(right + left) / (right - left)
      , 0.0f                 , 2.0f / (top - bottom), 0.0f                 , -(top + bottom) / (top - bottom)
      , 0.0f                 , 0.0f                 , 2.0f / (zNear - zFar), (zNear + zFar) / (zNear - zFar)
      , 0.0f                 , 0.0f                 , 0.0f                 , 1.0f);
}

void veMat4::makePerspective(float fovy, float aspectRatio, float zNear, float zFar)
{
    float top = zNear * tan(veMath::veRadian(fovy * 0.5f));
    float bottom = -top;
    float right = top * aspectRatio;
    float left = -right;
    float a = -(zFar + zNear) / (zFar - zNear);
    float b = -(2.0 * zNear * zFar) / (zFar - zNear);
    
    set((2.0f * zNear) / (right - left), 0.0f                           , (right + left) / (right - left), 0.0f
       , 0.0f                          , (2.0f * zNear) / (top - bottom), (top + bottom) / (top - bottom), 0.0f
       , 0.0f                          , 0.0f                           , a                              , b
       , 0.0f                          , 0.0f                           , -1.0f                          , 0.0f);
}

void veMat4::decomposition(veVec3* position, veVec3* scale, veQuat* orientation) const
{
	if (position){
		position->x() = m[0][3];
		position->y() = m[1][3];
		position->z() = m[2][3];
	}

	if (scale || orientation){
        veMat3 m3x3(m[0][0], m[0][1], m[0][2]
                  , m[1][0], m[1][1], m[1][2]
                  , m[2][0], m[2][1], m[2][2]);
        veMat3 kQ;
        // Factor M = QR = QDU where Q is orthogonal, D is diagonal,
        // and U is upper triangular with ones on its diagonal.  Algorithm uses
        // Gram-Schmidt orthogonalization (the QR algorithm).
        //
        // If M = [ m0 | m1 | m2 ] and Q = [ q0 | q1 | q2 ], then
        //
        //   q0 = m0/|m0|
        //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
        //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
        //
        // where |V| indicates length of vector V and A*B indicates dot
        // product of vectors A and B.  The matrix R has entries
        //
        //   r00 = q0*m0  r01 = q0*m1  r02 = q0*m2
        //   r10 = 0      r11 = q1*m1  r12 = q1*m2
        //   r20 = 0      r21 = 0      r22 = q2*m2
        //
        // so D = diag(r00,r11,r22) and U has entries u01 = r01/r00,
        // u02 = r02/r00, and u12 = r12/r11.
        
        // Q = rotation
        // D = scaling
        // U = shear
        
        // D stores the three diagonal entries r00, r11, r22
        // U stores the entries U[0] = u01, U[1] = u02, U[2] = u12
        
        // build orthogonal matrix Q
        veReal fInvLength = m3x3[0][0] * m3x3[0][0] + m3x3[1][0] * m3x3[1][0] + m3x3[2][0] * m3x3[2][0];
        if (fInvLength != 0.0f) fInvLength = 1.0f / veMath::veSqrt(fInvLength);
        
        kQ[0][0] = m3x3[0][0] * fInvLength;
        kQ[1][0] = m3x3[1][0] * fInvLength;
        kQ[2][0] = m3x3[2][0] * fInvLength;
        
        veReal fDot = kQ[0][0] * m3x3[0][1] + kQ[1][0] * m3x3[1][1] + kQ[2][0] * m3x3[2][1];
        kQ[0][1] = m3x3[0][1]-fDot * kQ[0][0];
        kQ[1][1] = m3x3[1][1]-fDot * kQ[1][0];
        kQ[2][1] = m3x3[2][1]-fDot * kQ[2][0];
        fInvLength = kQ[0][1] * kQ[0][1] + kQ[1][1] * kQ[1][1] + kQ[2][1] * kQ[2][1];
        if (fInvLength != 0.0f) fInvLength = 1.0f / veMath::veSqrt(fInvLength);
        
        kQ[0][1] *= fInvLength;
        kQ[1][1] *= fInvLength;
        kQ[2][1] *= fInvLength;
        
        fDot = kQ[0][0] * m3x3[0][2] + kQ[1][0] * m3x3[1][2] + kQ[2][0] * m3x3[2][2];
        kQ[0][2] = m3x3[0][2]-fDot * kQ[0][0];
        kQ[1][2] = m3x3[1][2]-fDot * kQ[1][0];
        kQ[2][2] = m3x3[2][2]-fDot * kQ[2][0];
        fDot = kQ[0][1] * m3x3[0][2] + kQ[1][1] * m3x3[1][2] + kQ[2][1] * m3x3[2][2];
        kQ[0][2] -= fDot * kQ[0][1];
        kQ[1][2] -= fDot * kQ[1][1];
        kQ[2][2] -= fDot * kQ[2][1];
        fInvLength = kQ[0][2] * kQ[0][2] + kQ[1][2] * kQ[1][2] + kQ[2][2] * kQ[2][2];
        if (fInvLength != 0.0f) fInvLength = 1.0f / veMath::veSqrt(fInvLength);
        
        kQ[0][2] *= fInvLength;
        kQ[1][2] *= fInvLength;
        kQ[2][2] *= fInvLength;
        
        // guarantee that orthogonal matrix has determinant 1 (no reflections)
        veReal fDet = kQ[0][0] * kQ[1][1] * kQ[2][2] + kQ[0][1] * kQ[1][2] * kQ[2][0] +
        kQ[0][2] * kQ[1][0] * kQ[2][1] - kQ[0][2] * kQ[1][1] * kQ[2][0] -
        kQ[0][1] * kQ[1][0] * kQ[2][2] - kQ[0][0] * kQ[1][2] * kQ[2][1];
        
        if ( fDet < 0.0 )
        {
            for (size_t iRow = 0; iRow < 3; iRow++)
                for (size_t iCol = 0; iCol < 3; iCol++)
                    kQ[iRow][iCol] = -kQ[iRow][iCol];
        }
        
        if (scale) {
            // the scaling component
            scale->x() = kQ[0][0] * m3x3[0][0] + kQ[1][0] * m3x3[1][0] + kQ[2][0] * m3x3[2][0];
            scale->y() = kQ[0][1] * m3x3[0][1] + kQ[1][1] * m3x3[1][1] + kQ[2][1] * m3x3[2][1];
            scale->z() = kQ[0][2] * m3x3[0][2] + kQ[1][2] * m3x3[1][2] + kQ[2][2] * m3x3[2][2];
        }
        
        if (orientation) {
            orientation->set(kQ);
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

	veReal Det = t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03;
	if (veMath::veAbs(Det) < veMath::TOLERANCE)
		return;
	veReal invDet = 1 / Det;

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

veMat4 veMat4::ortho(float left, float right, float bottom, float top, float zNear, float zFar)
{
    veMat4 m;
    m.makeOrtho(left, right, bottom, top, zNear, zFar);
    return m;
}

veMat4 veMat4::perspective(float fovy, float aspectRatio, float zNear, float zFar)
{
    veMat4 m;
    m.makePerspective(fovy, aspectRatio, zNear, zFar);
    return m;
}
