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

#define  VE_SAFE_DELETE(_PTR) do { if (_PTR){ delete _PTR; _PTR = nullptr; } } while (false);

#define VE_CALLBACK_0(__selector__,__target__, ...) std::bind(&__selector__,__target__, ##__VA_ARGS__)
#define VE_CALLBACK_1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
#define VE_CALLBACK_2(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define VE_CALLBACK_3(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)

#if defined(_MSC_VER)
#include <assert.h>
#define veAssert(_Expression)     assert(_Expression)
#else
#define veAssert(_Expression)     ((void)0)
#endif

#include <memory.h>
#include <vector>
#include <string>

#include "glew/include/GL/glew.h"
#include "glfw/include/GLFW/glfw3.h"

#endif
