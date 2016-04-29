#include "MatrixPtr.h"

veMat4Ptr::veMat4Ptr(const veMat4 &mat)
	: USE_VE_PTR_INIT
	, _mat(mat)
{

}

veMat4Ptr::~veMat4Ptr()
{

}
