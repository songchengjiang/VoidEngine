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

veApplication::veApplication()
	: _isRunning(false)
{
}

veApplication::~veApplication()
{
	stop();
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

void veApplication::initWindow(int w, int h, const std::string &title, void *param)
{
	_width = w;
	_height = h;
	_title = title;
    _currentEvent.setEventType(veEvent::VE_WIN_INIT);
    _currentEvent.setWindowWidth(_width);
    _currentEvent.setWindowHeight(_height);
    _events.push_back(_currentEvent);
	initWindowImplementation(param);
}

void veApplication::setSceneManager(veSceneManager *sm)
{
	if (_sceneManager.valid())
		_sceneManager->stopThreading();
	_sceneManager = sm;
}

void veApplication::stop()
{
	_isRunning = false;
}
