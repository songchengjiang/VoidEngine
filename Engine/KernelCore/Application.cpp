#include "Application.h"
#include "OctreeSceneManager.h"
#include <time.h>
#if (VE_PLATFORM == VE_PLATFORM_WIN32) || (VE_PLATFORM == VE_PLATFORM_MAC)
#include "Application-desktop.h"
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
	initWindowImplementation(param);
}

void veApplication::setSceneManager(veSceneManager *sm)
{
	if (_sceneManager.valid())
		_sceneManager->stopThreading();
	_sceneManager = sm;
}

bool veApplication::run()
{
	if (!_sceneManager.valid()) return false;
    _isRunning = true;
	_sceneManager->startThreading();
	double preFrameTime = (double)clock() / (double)CLOCKS_PER_SEC;
	while (_isRunning && !isWindowShouldClose())
	{
		double currentFrameTime = (double)clock() / (double)CLOCKS_PER_SEC;
		_sceneManager->setDeltaTime(currentFrameTime - preFrameTime);
		this->dispatchEvents();
		_sceneManager->simulation();
		preFrameTime = currentFrameTime;
	}
	_sceneManager->stopThreading();
	return true;
}

void veApplication::stop()
{
	_isRunning = false;
}
