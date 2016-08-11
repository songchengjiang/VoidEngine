#include "Application-desktop.h"
#if (VE_PLATFORM == VE_PLATFORM_WIN32) || (VE_PLATFORM == VE_PLATFORM_MAC)
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

bool veApplicationDesktop::run()
{
    if (_viewerList.empty()) return false;
    _isRunning = true;
    
    for (auto &viewer : _viewerList) {
        viewer->create();
        viewer->startSimulation();
    }
    while (_isRunning && !_viewerList.empty()) {
        glfwPollEvents();
        updateViewers();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    stop();
    return true;
}

void veApplicationDesktop::stop()
{
    _isRunning = false;
    for (auto &viewer : _viewerList){
        viewer->stopSimulation();
        viewer->destroy();
    }
}

void veApplicationDesktop::updateViewers()
{
    for (auto viewer = _viewerList.begin(); viewer != _viewerList.end();){
        if (glfwWindowShouldClose(static_cast<veViewerDesktop *>(*viewer)->_hwnd) != 0){
            (*viewer)->stopSimulation();
            (*viewer)->destroy();
            viewer = _viewerList.erase(viewer);
        }else{
            ++viewer;
        }
    }
}

#endif