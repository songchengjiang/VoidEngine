#ifndef _VE_PREREQUISITES_
#define _VE_PREREQUISITES_
#include "Export.h"

#define VE_VERSION_MAJOR 0
#define VE_VERSION_MINOR 1
#define VE_VERSION_PATCH 0
#define VE_VERSION_SUFFIX ""
#define VE_VERSION_NAME "Born"

#define VE_VERSION    ((VE_VERSION_MAJOR << 16) | (VE_VERSION_MINOR << 8) | VE_VERSION_PATCH)

// define the real number values to be used
// default to use 'float' unless precompiler option set
#if VE_DOUBLE_PRECISION == 1
/** Software floating point type.
@note Not valid as a pointer to GPU buffers / parameters
*/
typedef double veReal;
#else
/** Software floating point type.
@note Not valid as a pointer to GPU buffers / parameters
*/
typedef float veReal;
#endif


#define veAssert(_Expression)     ((void)0)

#endif
