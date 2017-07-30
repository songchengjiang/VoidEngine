#include "Application.h"
#include "OctreeSceneManager.h"
#include <time.h>
#if (VE_PLATFORM == VE_PLATFORM_WIN32) || (VE_PLATFORM == VE_PLATFORM_MAC)
#include "Application-desktop.h"
#elif VE_PLATFORM == VE_PLATFORM_IOS
#include "Application-ios.h"
#elif VE_PLATFORM == VE_PLATFORM_ANDROID
#include "Application-android.h"
#endif

#include "Viewer.h"

veApplication::veApplication()
{
}

veApplication::~veApplication()
{
}

veApplication* veApplication::instance()
{
#if (VE_PLATFORM == VE_PLATFORM_WIN32) || (VE_PLATFORM == VE_PLATFORM_MAC)
	static veApplicationDesktop application;
#elif VE_PLATFORM == VE_PLATFORM_IOS
    static veApplicationIOS application;
#elif VE_PLATFORM == VE_PLATFORM_ANDROID
	static veApplicationAndroid application;
#endif
	return &application;
}

veViewer* veApplication::getViewer(size_t idx)
{
    veAssert(idx < _viewerList.size());
    return _viewerList[idx];
}

void veApplication::destroyAllViewer()
{
    stop();
    _viewerList.clear();
}
