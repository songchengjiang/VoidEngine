#include "Matrix3.h"

const veMat3 veMat3::IDENTITY = veMat3(1.0f, 0.0f, 0.0f
									 , 0.0f, 1.0f, 0.0f
									 , 0.0f, 0.0f, 1.0f);

veMat3::veMat3()
{

}

veMat3::veMat3(const veMat3 &copy)
{

}

veMat3::veMat3(const veQuat &rot)
{

}

veMat3::veMat3(veReal m00, veReal m01, veReal m02
	, veReal m10, veReal m11, veReal m12
	, veReal m20, veReal m21, veReal m22)
{
	set(m00, m01, m02
		, m10, m11, m12
		, m20, m21, m22);
}

veMat3::~veMat3()
{

}

void veMat3::identity()
{
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
}

void veMat3::set(veReal m00, veReal m01, veReal m02
	, veReal m10, veReal m11, veReal m12
	, veReal m20, veReal m21, veReal m22)
{
	m[0][0] = m00;
	m[0][1] = m01;
	m[0][2] = m02;
	m[1][0] = m10;
	m[1][1] = m11;
	m[1][2] = m12;
	m[2][0] = m20;
	m[2][1] = m21;
	m[2][2] = m22;
}

void veMat3::transpose()
{
	set(m[0][0], m[1][0], m[2][0],
		m[0][1], m[1][1], m[2][1],
		m[0][2], m[1][2], m[2][2]);
}

void veMat3::inverse()
{
	// Invert a 3x3 using cofactors.  This is about 8 times faster than
	// the Numerical Recipes code which uses Gaussian elimination.
	veReal d00 = m[1][1] * m[2][2] - m[1][2] * m[2][1];
	veReal d01 = m[0][2] * m[2][1] - m[0][1] * m[2][2];
	veReal d02 = m[0][1] * m[1][2] - m[0][2] * m[1][1];
	veReal d10 = m[1][2] * m[2][0] - m[1][0] * m[2][2];
	veReal d11 = m[0][0] * m[2][2] - m[0][2] * m[2][0];
	veReal d12 = m[0][2] * m[1][0] - m[0][0] * m[1][2];
	veReal d20 = m[1][0] * m[2][1] - m[1][1] * m[2][0];
	veReal d21 = m[0][1] * m[2][0] - m[0][0] * m[2][1];
	veReal d22 = m[0][0] * m[1][1] - m[0][1] * m[1][0];

	veReal fDet = m[0][0] * d00 + m[0][1] * d10 + m[0][2] * d20;

	veReal fInvDet = 1.0f / fDet;
	set(d00 * fInvDet, d01 * fInvDet, d02 * fInvDet
		, d10 * fInvDet, d11 * fInvDet, d12 * fInvDet
		, d20 * fInvDet, d21 * fInvDet, d22 * fInvDet);
}
