#include "MatrixPtr.h"

veMat4Ptr::veMat4Ptr(const veMat4 *mat, size_t size)
	: USE_VE_PTR_INIT
    , _size(size)
{
    _mat = new veMat4[size];
    memcpy(_mat, mat, sizeof(veMat4) * size);
}

veMat4Ptr::veMat4Ptr(size_t size)
	: USE_VE_PTR_INIT
    , _size(size)
{
    _mat = new veMat4[size];
}

veMat4Ptr::~veMat4Ptr()
{
    delete[] _mat;
}
