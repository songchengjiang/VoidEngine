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

#define VE_N_BYTE_ALIGN(_BYTE, _N) ((_BYTE + _N - 1) & (~(_N - 1)))

#define USE_VE_PTR \
protected: size_t _inUse;\
public: void ref() { ++_inUse; }\
public: size_t unRef() { --_inUse; return _inUse; }\
public: size_t refCount() { return _inUse; }

#define USE_VE_PTR_INIT _inUse(0)

#define  USE_NAME_PROPERTY \
protected: std::string _name;\
public: void setName(const std::string &name) { _name = name; } \
public: const std::string& getName() const { return _name; }

//#define  USE_ALIAS_NAME_PROPERTY \
//protected: std::string _aliasName;\
//protected: void setAliasName(const std::string &name) { _aliasName = name; } \
//public: const std::string& getAliasName() const { return _aliasName; }

#if defined(_MSC_VER)
#if defined(_DEBUG)
#include <assert.h>
#define veAssert(_Expression)     assert(_Expression)
#else
#define veAssert(_Expression)     ((void)0)
#endif
#else
#define veAssert(_Expression)     ((void)0)
#endif

#if defined(_MSC_VER)
#if defined(_DEBUG)
#include <stdio.h>
#define veLog(...) printf(__VA_ARGS__);
#else
#define veLog(...) ((void)0);
#endif
#elif defined(__APPLE_CC__)
#include <stdio.h>
#define veLog(...) printf(__VA_ARGS__);
#elif defined(ANDROID)
#include <android/log.h>
#define veLogI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "VoidEngine", __VA_ARGS__))
#define veLogW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "VoidEngine", __VA_ARGS__))
#define veLog veLogI
#else
#define veLog(...) ((void)0);
#endif

#include <memory.h>
#include <vector>
#include <string>

#define VE_GL_VERSION_MAJOR 4
#define VE_GL_VERSION_MINOR 1

#define VE_GLSL_ES_VERSION_MAJOR 3
#define VE_GLSL_ES_VERSION_MINOR 0

#define VE_PLATFORM_IOS                1
#define VE_PLATFORM_ANDROID            2
#define VE_PLATFORM_WIN32              3
#define VE_PLATFORM_LINUX              4
#define VE_PLATFORM_MAC                5
#define VE_PLATFORM_UNKNOW             6

#if defined(_MSC_VER)
    #define VE_PLATFORM     VE_PLATFORM_WIN32
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE // TARGET_OS_IPHONE inlcudes TARGET_OS_IOS TARGET_OS_TV and TARGET_OS_WATCH. see TargetConditionals.h
        #define VE_PLATFORM         VE_PLATFORM_IOS
    #elif TARGET_OS_MAC
        #define VE_PLATFORM         VE_PLATFORM_MAC
    #endif
#elif defined(ANDROID)
    #define VE_PLATFORM     VE_PLATFORM_ANDROID
#else
    #define VE_PLATFORM     VE_PLATFORM_UNKNOW
#endif


#if VE_PLATFORM == VE_PLATFORM_WIN32 || VE_PLATFORM == VE_PLATFORM_MAC
#if VE_PLATFORM == VE_PLATFORM_WIN32
#define GLEW_STATIC
#define GLEW_MX
#include "glew/include/GL/glew.h"
extern GLEWContext* glewGetContext();
#endif

#if VE_PLATFORM == VE_PLATFORM_MAC
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_INCLUDE_GLEXT
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#endif
#include "glfw/include/GLFW/glfw3.h"
#endif

#if VE_PLATFORM == VE_PLATFORM_IOS
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#endif

#if VE_PLATFORM == VE_PLATFORM_ANDROID
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#endif

#endif
