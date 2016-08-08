#include "Application-desktop.h"
#if (VE_PLATFORM == VE_PLATFORM_WIN32) || (VE_PLATFORM == VE_PLATFORM_MAC)
#if (VE_PLATFORM == VE_PLATFORM_WIN32)
#include <windows.h>
#endif
#include "Viewer-desktop.h"

veApplicationDesktop::veApplicationDesktop()
    : _isRunning(false)
{
	glfwInit();
#if (VE_PLATFORM == VE_PLATFORM_MAC)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, VE_GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, VE_GL_VERSION_MINOR);
#endif
}

veViewer* veApplicationDesktop::createViewer(int width, int height, const std::string &title, veViewer *sharedContextViewer)
{
    auto viewer = new veViewerDesktop(width, height, title, static_cast<veViewerDesktop *>(sharedContextViewer));
    _viewerList.push_back(viewer);
    return viewer;
}

veApplicationDesktop::~veApplicationDesktop()
{
    stop();
    for (auto &viewer : _viewerList){
        VE_SAFE_DELETE(viewer);
    }
	glfwTerminate();
}

#if (VE_PLATFORM == VE_PLATFORM_WIN32)
bool veApplicationDesktop::run()
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
    
    for (auto &viewer : _viewerList){
        viewer->startRender(_sceneManager.get());
    }
	while (_isRunning && !_viewerList.empty())
	{
		LARGE_INTEGER currentFrameTime;
		QueryPerformanceCounter(&currentFrameTime);
		if (frameTimeLimit.QuadPart <= (currentFrameTime.QuadPart - preFrameTime.QuadPart)) {
            glfwPollEvents();
            _sceneManager->update((currentFrameTime.QuadPart - preFrameTime.QuadPart) * frequencyPreSec, &_eventDispatcher);
            _eventDispatcher.clearEventList();
            updateViewers();
            
			preFrameTime.QuadPart = currentFrameTime.QuadPart;
		}
		else {
			//std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	stop();

	return true;
}
#endif

#if (VE_PLATFORM == VE_PLATFORM_MAC)
bool veApplicationDesktop::run()
{
    if (_viewerList.empty()) return false;
    _isRunning = true;
    clock_t frameTimeClocks = 1.0 / 60.0 * CLOCKS_PER_SEC;
    clock_t preFrameTime = clock();
    double invertClocksSec = 1.0 / (double)CLOCKS_PER_SEC;
    
    for (auto &viewer : _viewerList){
        viewer->create();
        viewer->startRender();
    }
    while (_isRunning && !_viewerList.empty())
    {
        clock_t currentFrameTime = clock();
        glfwPollEvents();
        
        for (auto &viewer : _viewerList){
            viewer->simulation((currentFrameTime - preFrameTime) * invertClocksSec);
        }
        updateViewers();
        //veLog("Frame Rate: %f\n", 1.0 / _sceneManager->getDeltaTime());
        while ((clock() - currentFrameTime) < frameTimeClocks) {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
        preFrameTime = currentFrameTime;
    }
    stop();
    return true;
}
#endif

void veApplicationDesktop::stop()
{
    _isRunning = false;
    for (auto &viewer : _viewerList){
        viewer->stopRender();
        viewer->destroy();
    }
}

void veApplicationDesktop::updateViewers()
{
    for (auto viewer = _viewerList.begin(); viewer != _viewerList.end();){
        if (glfwWindowShouldClose(static_cast<veViewerDesktop *>(*viewer)->_hwnd) != 0){
            (*viewer)->stopRender();
            (*viewer)->destroy();
            viewer = _viewerList.erase(viewer);
        }else{
            ++viewer;
        }
    }
}

#endif