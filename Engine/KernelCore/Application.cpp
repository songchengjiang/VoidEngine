#include "Application.h"
#include "OctreeSceneManager.h"
#include <time.h>
#if (VE_PLATFORM == VE_PLATFORM_WIN32) || (VE_PLATFORM == VE_PLATFORM_MAC)
#include "Application-desktop.h"
#if (VE_PLATFORM == VE_PLATFORM_WIN32)
#include <windows.h>
#endif
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

#if (VE_PLATFORM == VE_PLATFORM_WIN32)
bool veApplication::run()
{
	if (!_sceneManager.valid()) return false;
    _isRunning = true;

	LARGE_INTEGER  frequency;
	LARGE_INTEGER frameTimeLimit;
	LARGE_INTEGER preFrameTime;
	QueryPerformanceFrequency(&frequency);
	frameTimeLimit.QuadPart = (1.0 / 60.0) * frequency.QuadPart;
	QueryPerformanceCounter(&preFrameTime);
	double frequencyPreSec = 1.0 / frequency.QuadPart;
	_sceneManager->startThreading();
	while (_isRunning && !isWindowShouldClose())
	{
		LARGE_INTEGER currentFrameTime;
		QueryPerformanceCounter(&currentFrameTime);
		if (frameTimeLimit.QuadPart <= (currentFrameTime.QuadPart - preFrameTime.QuadPart)) {
			_sceneManager->setDeltaTime((currentFrameTime.QuadPart - preFrameTime.QuadPart) * frequencyPreSec);
			//veLog("DeltaTime: %f\n", _sceneManager->getDeltaTime());
			this->dispatchEvents();
			_sceneManager->simulation();
			preFrameTime.QuadPart = currentFrameTime.QuadPart;
		}else {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
	_sceneManager->stopThreading();

	return true;
}
#endif

void veApplication::stop()
{
	_isRunning = false;
}
