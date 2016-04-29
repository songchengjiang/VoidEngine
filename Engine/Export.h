#ifndef _VE_EXPORT_
#define _VE_EXPORT_
// define USE_DEPRECATED_API is used to include in API which is being fazed out
// if you can compile your apps with this turned off you are
// well placed for compatibility with future versions.
#define USE_DEPRECATED_API

// disable VisualStudio warnings
#if defined(_MSC_VER) && defined(VE_DISABLE_MSVC_WARNINGS)
#pragma warning( disable : 4244 )
#pragma warning( disable : 4251 )
#pragma warning( disable : 4275 )
#pragma warning( disable : 4512 )
#pragma warning( disable : 4267 )
#pragma warning( disable : 4702 )
#pragma warning( disable : 4511 )
#pragma warning( disable : 4005 )
#endif

#if defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__) || defined( __BCPLUSPLUS__)  || defined( __MWERKS__)
#  if defined( VE_LIBRARY_STATIC )
#    define VE_EXPORT
#  elif defined( VE_LIBRARY )
#    define VE_EXPORT   __declspec(dllexport)
#  else
#    define VE_EXPORT   __declspec(dllimport)
#  endif
#else
#  define VE_EXPORT
#endif

// set up define for whether member templates are supported by VisualStudio compilers.
#ifdef _MSC_VER
# if (_MSC_VER >= 1300)
#  define __STL_MEMBER_TEMPLATES
# endif
#endif

#endif

