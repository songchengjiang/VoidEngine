#include "Viewer-desktop.h"
#if (VE_PLATFORM == VE_PLATFORM_WIN32) || (VE_PLATFORM == VE_PLATFORM_MAC)
#include "Application-desktop.h"
#include "SceneManager.h"

veViewerDesktop::veViewerDesktop(int width, int height, const std::string &title, GLFWwindow *sharedHwnd)
    : veViewer(width, height, title)
    , _isInited(false)
    , _isRendering(false)
    , _hwnd(nullptr)
    , _sharedHwnd(sharedHwnd)
{
}

veViewerDesktop::~veViewerDesktop()
{
    
}

bool veViewerDesktop::makeContextCurrent()
{
    glfwMakeContextCurrent(_hwnd);
    if (!_isInited) {
#if VE_PLATFORM == VE_PLATFORM_WIN32
        if (glewInit() != GLEW_OK) {
            veLog("glewInit error!");
            return false;
        }
#endif
        _isInited = true;
    }
    
    return true;
}

void veViewerDesktop::swapBuffers()
{
    glfwSwapBuffers(_hwnd);
}

void veViewerDesktop::startRender(veSceneManager *sm)
{
    if (_isRendering) return;
    _isRendering = true;
    _renderingThread = std::thread([sm, this] {
        while(_isRendering && _hwnd){
            sm->render(this);
        }
    });
}

void veViewerDesktop::stopRender(veSceneManager *sm)
{
    if (!_isRendering) return;
    _isRendering = false;
    _renderingThread.join();
}

void veViewerDesktop::create()
{
    if (_hwnd) return;
    _hwnd = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, _sharedHwnd);
    
    glfwSetWindowUserPointer(_hwnd, veApplication::instance());
    glfwSetKeyCallback(_hwnd, veApplicationDesktop::collectKeyEvent);
    glfwSetCharCallback(_hwnd, veApplicationDesktop::collectCharEvent);
    glfwSetMouseButtonCallback(_hwnd, veApplicationDesktop::collectMouseEvent);
    glfwSetCursorPosCallback(_hwnd, veApplicationDesktop::collectMouseMoveEvent);
    glfwSetScrollCallback(_hwnd, veApplicationDesktop::collectScrollEvent);
    glfwSetWindowSizeCallback(_hwnd, veApplicationDesktop::collectWindowSizeEvent);
    glfwSetWindowFocusCallback(_hwnd, veApplicationDesktop::collectWindowFocusEvent);
    glfwSetWindowCloseCallback(_hwnd, veApplicationDesktop::collectWindowClose);
}

void veViewerDesktop::destroy()
{
    if (!_hwnd) return;
    glfwSetKeyCallback(_hwnd, nullptr);
    glfwSetCharCallback(_hwnd, nullptr);
    glfwSetMouseButtonCallback(_hwnd, nullptr);
    glfwSetCursorPosCallback(_hwnd, nullptr);
    glfwSetScrollCallback(_hwnd, nullptr);
    glfwSetWindowSizeCallback(_hwnd, nullptr);
    glfwSetWindowFocusCallback(_hwnd, nullptr);
    glfwSetWindowCloseCallback(_hwnd, nullptr);
    
    glfwDestroyWindow(_hwnd);
    _hwnd = nullptr;
}

void veViewerDesktop::show()
{
    glfwShowWindow(_hwnd);
}

void veViewerDesktop::hide()
{
    glfwHideWindow(_hwnd);
}

bool veViewerDesktop::isNeedClosed() const
{
    return glfwWindowShouldClose(_hwnd) != 0;
}

#endif