#ifndef _VE_MATRIX_PTR_
#define _VE_MATRIX_PTR_
#include "Prerequisites.h"
#include "BaseCore/Matrix4.h"

class veMat4Ptr
{
public:
	veMat4Ptr(const veMat4 &mat);
	~veMat4Ptr();

	USE_VE_PTR;

	const veMat4& value() const { return _mat; }
	veMat4& value() { return _mat; }

private:

	veMat4 _mat;
};

#endif