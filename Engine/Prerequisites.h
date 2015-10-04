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
#define  VE_SAFE_DELETE_ARRAY(_PTR) do { if (_PTR){ delete[] _PTR; _PTR = nullptr; } } while (false);

#define VE_CALLBACK_0(__selector__,__target__, ...) std::bind(&__selector__,__target__, ##__VA_ARGS__)
#define VE_CALLBACK_1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
#define VE_CALLBACK_2(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define VE_CALLBACK_3(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)

#define USE_VE_PTR \
protected: size_t _inUse;\
public: void ref() { ++_inUse; }\
public: size_t unRef() { --_inUse; return _inUse; }

#define USE_VE_PTR_INIT _inUse(0)

#define  USE_NAME_PROPERTY \
protected: std::string _name;\
public: void setName(const std::string &name) { _name = name; } \
public: const std::string& getName() const { return _name; }

#if defined(_MSC_VER)
#include <assert.h>
#define veAssert(_Expression)     assert(_Expression)
#else
#define veAssert(_Expression)     ((void)0)
#endif

#if defined(_DEBUG)
#include <iostream>
#define veLog(STR) std::cout<<STR<<std::endl; 
#elif defined(__APPLE_CC__)
#include <iostream>
#define veLog(STR) std::cout<<STR<<std::endl;
#endif

#include <memory.h>
#include <vector>
#include <string>

#if defined(__APPLE_CC__)
#define VE_DEVICE_PIXEL_RATIO 2
#else
#define VE_DEVICE_PIXEL_RATIO 1
#endif

#define VE_GLSL_VERSION_MAJOR 4
#define VE_GLSL_VERSION_MINOR 1


#if defined(_MSC_VER)
#define GLEW_STATIC
#include "glew/include/GL/glew.h"
#endif

#if defined(__APPLE_CC__)
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#endif
#include "glfw/include/GLFW/glfw3.h"

#endif
