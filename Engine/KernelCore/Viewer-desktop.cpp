#include "Viewer-desktop.h"
#if (VE_PLATFORM == VE_PLATFORM_WIN32) || (VE_PLATFORM == VE_PLATFORM_MAC)
#include "SceneManager.h"
#if (VE_PLATFORM == VE_PLATFORM_WIN32)
#include <windows.h>
#endif

static std::unordered_map<int, veEvent::KeySymbol> g_KeySymbolMap;
static std::unordered_map<int, veEvent::ModKeySymbol> g_ModKeySymbolMap;
static std::unordered_map<int, veEvent::MouseSymbol> g_MouseSymbolMap;

static void initSymbolsMap() {
    if (!g_KeySymbolMap.empty()) return;
    g_KeySymbolMap[GLFW_KEY_UNKNOWN] = veEvent::VE_KEY_UNKNOWN;
    
    /* Printable keys */
    g_KeySymbolMap[GLFW_KEY_SPACE] = veEvent::VE_KEY_SPACE;
    g_KeySymbolMap[GLFW_KEY_APOSTROPHE] = veEvent::VE_KEY_APOSTROPHE;
    g_KeySymbolMap[GLFW_KEY_COMMA] = veEvent::VE_KEY_COMMA;
    g_KeySymbolMap[GLFW_KEY_MINUS] = veEvent::VE_KEY_MINUS;
    g_KeySymbolMap[GLFW_KEY_PERIOD] = veEvent::VE_KEY_PERIOD;
    g_KeySymbolMap[GLFW_KEY_SLASH] = veEvent::VE_KEY_SLASH;
    g_KeySymbolMap[GLFW_KEY_0] = veEvent::VE_KEY_0;
    g_KeySymbolMap[GLFW_KEY_1] = veEvent::VE_KEY_1;
    g_KeySymbolMap[GLFW_KEY_2] = veEvent::VE_KEY_2;
    g_KeySymbolMap[GLFW_KEY_3] = veEvent::VE_KEY_3;
    g_KeySymbolMap[GLFW_KEY_4] = veEvent::VE_KEY_4;
    g_KeySymbolMap[GLFW_KEY_5] = veEvent::VE_KEY_5;
    g_KeySymbolMap[GLFW_KEY_6] = veEvent::VE_KEY_6;
    g_KeySymbolMap[GLFW_KEY_7] = veEvent::VE_KEY_7;
    g_KeySymbolMap[GLFW_KEY_8] = veEvent::VE_KEY_8;
    g_KeySymbolMap[GLFW_KEY_9] = veEvent::VE_KEY_9;
    g_KeySymbolMap[GLFW_KEY_SEMICOLON] = veEvent::VE_KEY_SEMICOLON;
    g_KeySymbolMap[GLFW_KEY_EQUAL] = veEvent::VE_KEY_EQUAL;
    g_KeySymbolMap[GLFW_KEY_A] = veEvent::VE_KEY_A;
    g_KeySymbolMap[GLFW_KEY_B] = veEvent::VE_KEY_B;
    g_KeySymbolMap[GLFW_KEY_C] = veEvent::VE_KEY_C;
    g_KeySymbolMap[GLFW_KEY_D] = veEvent::VE_KEY_D;
    g_KeySymbolMap[GLFW_KEY_E] = veEvent::VE_KEY_E;
    g_KeySymbolMap[GLFW_KEY_F] = veEvent::VE_KEY_F;
    g_KeySymbolMap[GLFW_KEY_G] = veEvent::VE_KEY_G;
    g_KeySymbolMap[GLFW_KEY_H] = veEvent::VE_KEY_H;
    g_KeySymbolMap[GLFW_KEY_I] = veEvent::VE_KEY_I;
    g_KeySymbolMap[GLFW_KEY_J] = veEvent::VE_KEY_J;
    g_KeySymbolMap[GLFW_KEY_K] = veEvent::VE_KEY_K;
    g_KeySymbolMap[GLFW_KEY_L] = veEvent::VE_KEY_L;
    g_KeySymbolMap[GLFW_KEY_M] = veEvent::VE_KEY_M;
    g_KeySymbolMap[GLFW_KEY_N] = veEvent::VE_KEY_N;
    g_KeySymbolMap[GLFW_KEY_O] = veEvent::VE_KEY_O;
    g_KeySymbolMap[GLFW_KEY_P] = veEvent::VE_KEY_P;
    g_KeySymbolMap[GLFW_KEY_Q] = veEvent::VE_KEY_Q;
    g_KeySymbolMap[GLFW_KEY_R] = veEvent::VE_KEY_R;
    g_KeySymbolMap[GLFW_KEY_S] = veEvent::VE_KEY_S;
    g_KeySymbolMap[GLFW_KEY_T] = veEvent::VE_KEY_T;
    g_KeySymbolMap[GLFW_KEY_U] = veEvent::VE_KEY_U;
    g_KeySymbolMap[GLFW_KEY_V] = veEvent::VE_KEY_V;
    g_KeySymbolMap[GLFW_KEY_W] = veEvent::VE_KEY_W;
    g_KeySymbolMap[GLFW_KEY_X] = veEvent::VE_KEY_X;
    g_KeySymbolMap[GLFW_KEY_Y] = veEvent::VE_KEY_Y;
    g_KeySymbolMap[GLFW_KEY_Z] = veEvent::VE_KEY_Z;
    g_KeySymbolMap[GLFW_KEY_LEFT_BRACKET] = veEvent::VE_KEY_LEFT_BRACKET;
    g_KeySymbolMap[GLFW_KEY_BACKSLASH] = veEvent::VE_KEY_BACKSLASH;
    g_KeySymbolMap[GLFW_KEY_RIGHT_BRACKET] = veEvent::VE_KEY_RIGHT_BRACKET;
    g_KeySymbolMap[GLFW_KEY_GRAVE_ACCENT] = veEvent::VE_KEY_GRAVE_ACCENT;
    g_KeySymbolMap[GLFW_KEY_WORLD_1] = veEvent::VE_KEY_WORLD_1;
    g_KeySymbolMap[GLFW_KEY_WORLD_2] = veEvent::VE_KEY_WORLD_2;
    
    /* Function keys */
    g_KeySymbolMap[GLFW_KEY_ESCAPE] = veEvent::VE_KEY_ESCAPE;
    g_KeySymbolMap[GLFW_KEY_ENTER] = veEvent::VE_KEY_ENTER;
    g_KeySymbolMap[GLFW_KEY_TAB] = veEvent::VE_KEY_TAB;
    g_KeySymbolMap[GLFW_KEY_BACKSPACE] = veEvent::VE_KEY_BACKSPACE;
    g_KeySymbolMap[GLFW_KEY_INSERT] = veEvent::VE_KEY_INSERT;
    g_KeySymbolMap[GLFW_KEY_DELETE] = veEvent::VE_KEY_DELETE;
    g_KeySymbolMap[GLFW_KEY_RIGHT] = veEvent::VE_KEY_RIGHT;
    g_KeySymbolMap[GLFW_KEY_LEFT] = veEvent::VE_KEY_LEFT;
    g_KeySymbolMap[GLFW_KEY_DOWN] = veEvent::VE_KEY_DOWN;
    g_KeySymbolMap[GLFW_KEY_UP] = veEvent::VE_KEY_UP;
    g_KeySymbolMap[GLFW_KEY_PAGE_UP] = veEvent::VE_KEY_PAGE_UP;
    g_KeySymbolMap[GLFW_KEY_PAGE_DOWN] = veEvent::VE_KEY_PAGE_DOWN;
    g_KeySymbolMap[GLFW_KEY_HOME] = veEvent::VE_KEY_HOME;
    g_KeySymbolMap[GLFW_KEY_END] = veEvent::VE_KEY_END;
    g_KeySymbolMap[GLFW_KEY_CAPS_LOCK] = veEvent::VE_KEY_CAPS_LOCK;
    g_KeySymbolMap[GLFW_KEY_SCROLL_LOCK] = veEvent::VE_KEY_SCROLL_LOCK;
    g_KeySymbolMap[GLFW_KEY_NUM_LOCK] = veEvent::VE_KEY_NUM_LOCK;
    g_KeySymbolMap[GLFW_KEY_PRINT_SCREEN] = veEvent::VE_KEY_PRINT_SCREEN;
    g_KeySymbolMap[GLFW_KEY_PAUSE] = veEvent::VE_KEY_PAUSE;
    g_KeySymbolMap[GLFW_KEY_F1] = veEvent::VE_KEY_F1;
    g_KeySymbolMap[GLFW_KEY_F2] = veEvent::VE_KEY_F2;
    g_KeySymbolMap[GLFW_KEY_F3] = veEvent::VE_KEY_F3;
    g_KeySymbolMap[GLFW_KEY_F4] = veEvent::VE_KEY_F4;
    g_KeySymbolMap[GLFW_KEY_F5] = veEvent::VE_KEY_F5;
    g_KeySymbolMap[GLFW_KEY_F6] = veEvent::VE_KEY_F6;
    g_KeySymbolMap[GLFW_KEY_F7] = veEvent::VE_KEY_F7;
    g_KeySymbolMap[GLFW_KEY_F8] = veEvent::VE_KEY_F8;
    g_KeySymbolMap[GLFW_KEY_F9] = veEvent::VE_KEY_F9;
    g_KeySymbolMap[GLFW_KEY_F10] = veEvent::VE_KEY_F10;
    g_KeySymbolMap[GLFW_KEY_F11] = veEvent::VE_KEY_F11;
    g_KeySymbolMap[GLFW_KEY_F12] = veEvent::VE_KEY_F12;
    g_KeySymbolMap[GLFW_KEY_F13] = veEvent::VE_KEY_F13;
    g_KeySymbolMap[GLFW_KEY_F14] = veEvent::VE_KEY_F14;
    g_KeySymbolMap[GLFW_KEY_F15] = veEvent::VE_KEY_F15;
    g_KeySymbolMap[GLFW_KEY_F16] = veEvent::VE_KEY_F16;
    g_KeySymbolMap[GLFW_KEY_F17] = veEvent::VE_KEY_F17;
    g_KeySymbolMap[GLFW_KEY_F18] = veEvent::VE_KEY_F18;
    g_KeySymbolMap[GLFW_KEY_F19] = veEvent::VE_KEY_F19;
    g_KeySymbolMap[GLFW_KEY_F20] = veEvent::VE_KEY_F20;
    g_KeySymbolMap[GLFW_KEY_F21] = veEvent::VE_KEY_F21;
    g_KeySymbolMap[GLFW_KEY_F22] = veEvent::VE_KEY_F22;
    g_KeySymbolMap[GLFW_KEY_F23] = veEvent::VE_KEY_F23;
    g_KeySymbolMap[GLFW_KEY_F24] = veEvent::VE_KEY_F24;
    g_KeySymbolMap[GLFW_KEY_F25] = veEvent::VE_KEY_F25;
    g_KeySymbolMap[GLFW_KEY_KP_0] = veEvent::VE_KEY_KP_0;
    g_KeySymbolMap[GLFW_KEY_KP_1] = veEvent::VE_KEY_KP_1;
    g_KeySymbolMap[GLFW_KEY_KP_2] = veEvent::VE_KEY_KP_2;
    g_KeySymbolMap[GLFW_KEY_KP_3] = veEvent::VE_KEY_KP_3;
    g_KeySymbolMap[GLFW_KEY_KP_4] = veEvent::VE_KEY_KP_4;
    g_KeySymbolMap[GLFW_KEY_KP_5] = veEvent::VE_KEY_KP_5;
    g_KeySymbolMap[GLFW_KEY_KP_6] = veEvent::VE_KEY_KP_6;
    g_KeySymbolMap[GLFW_KEY_KP_7] = veEvent::VE_KEY_KP_7;
    g_KeySymbolMap[GLFW_KEY_KP_8] = veEvent::VE_KEY_KP_8;
    g_KeySymbolMap[GLFW_KEY_KP_9] = veEvent::VE_KEY_KP_9;
    g_KeySymbolMap[GLFW_KEY_KP_DECIMAL] = veEvent::VE_KEY_KP_DECIMAL;
    g_KeySymbolMap[GLFW_KEY_KP_DIVIDE] = veEvent::VE_KEY_KP_DIVIDE;
    g_KeySymbolMap[GLFW_KEY_KP_MULTIPLY] = veEvent::VE_KEY_KP_MULTIPLY;
    g_KeySymbolMap[GLFW_KEY_KP_SUBTRACT] = veEvent::VE_KEY_KP_SUBTRACT;
    g_KeySymbolMap[GLFW_KEY_KP_ADD] = veEvent::VE_KEY_KP_ADD;
    g_KeySymbolMap[GLFW_KEY_KP_ENTER] = veEvent::VE_KEY_KP_ENTER;
    g_KeySymbolMap[GLFW_KEY_KP_EQUAL] = veEvent::VE_KEY_KP_EQUAL;
    g_KeySymbolMap[GLFW_KEY_LEFT_SHIFT] = veEvent::VE_KEY_LEFT_SHIFT;
    g_KeySymbolMap[GLFW_KEY_LEFT_CONTROL] = veEvent::VE_KEY_LEFT_CONTROL;
    g_KeySymbolMap[GLFW_KEY_LEFT_ALT] = veEvent::VE_KEY_LEFT_ALT;
    g_KeySymbolMap[GLFW_KEY_LEFT_SUPER] = veEvent::VE_KEY_LEFT_SUPER;
    g_KeySymbolMap[GLFW_KEY_RIGHT_SHIFT] = veEvent::VE_KEY_RIGHT_SHIFT;
    g_KeySymbolMap[GLFW_KEY_RIGHT_CONTROL] = veEvent::VE_KEY_RIGHT_CONTROL;
    g_KeySymbolMap[GLFW_KEY_RIGHT_ALT] = veEvent::VE_KEY_RIGHT_ALT;
    g_KeySymbolMap[GLFW_KEY_RIGHT_SUPER] = veEvent::VE_KEY_RIGHT_SUPER;
    g_KeySymbolMap[GLFW_KEY_MENU] = veEvent::VE_KEY_MENU;
    
    g_ModKeySymbolMap[GLFW_MOD_SHIFT] = veEvent::VE_MOD_SHIFT;
    g_ModKeySymbolMap[GLFW_MOD_CONTROL] = veEvent::VE_MOD_CONTROL;
    g_ModKeySymbolMap[GLFW_MOD_ALT] = veEvent::VE_MOD_ALT;
    g_ModKeySymbolMap[GLFW_MOD_SUPER] = veEvent::VE_MOD_SUPER;
    
    g_MouseSymbolMap[GLFW_MOUSE_BUTTON_1] = veEvent::VE_MOUSE_BUTTON_1;
    g_MouseSymbolMap[GLFW_MOUSE_BUTTON_2] = veEvent::VE_MOUSE_BUTTON_2;
    g_MouseSymbolMap[GLFW_MOUSE_BUTTON_3] = veEvent::VE_MOUSE_BUTTON_3;
    g_MouseSymbolMap[GLFW_MOUSE_BUTTON_4] = veEvent::VE_MOUSE_BUTTON_4;
    g_MouseSymbolMap[GLFW_MOUSE_BUTTON_5] = veEvent::VE_MOUSE_BUTTON_5;
    g_MouseSymbolMap[GLFW_MOUSE_BUTTON_6] = veEvent::VE_MOUSE_BUTTON_6;
    g_MouseSymbolMap[GLFW_MOUSE_BUTTON_7] = veEvent::VE_MOUSE_BUTTON_7;
    g_MouseSymbolMap[GLFW_MOUSE_BUTTON_8] = veEvent::VE_MOUSE_BUTTON_8;
    g_MouseSymbolMap[GLFW_MOUSE_BUTTON_LAST] = veEvent::VE_MOUSE_BUTTON_LAST;
    g_MouseSymbolMap[GLFW_MOUSE_BUTTON_LEFT] = veEvent::VE_MOUSE_BUTTON_LEFT;
    g_MouseSymbolMap[GLFW_MOUSE_BUTTON_RIGHT] = veEvent::VE_MOUSE_BUTTON_RIGHT;
    g_MouseSymbolMap[GLFW_MOUSE_BUTTON_MIDDLE] = veEvent::VE_MOUSE_BUTTON_MIDDLE;
    
}

#if VE_PLATFORM == VE_PLATFORM_WIN32
static GLEWContext *g_currentContext;
GLEWContext* glewGetContext()
{
    return g_currentContext;
}
#endif

veViewerDesktop::veViewerDesktop(int width, int height, const std::string &title, veViewerDesktop *sharedViewer)
    : veViewer(width, height, title)
    , _isInited(false)
    , _hwnd(nullptr)
    , _sharedViewer(sharedViewer)
{
    initSymbolsMap();
#if VE_PLATFORM == VE_PLATFORM_MAC
    if (_sharedViewer){
        _contextID = _sharedViewer->_contextID;
    }
#endif
#if VE_PLATFORM == VE_PLATFORM_WIN32
    _glContext = new GLEWContext;
#endif
}

veViewerDesktop::~veViewerDesktop()
{
#if VE_PLATFORM == VE_PLATFORM_WIN32
    VE_SAFE_DELETE(_glContext);
#endif
}

bool veViewerDesktop::makeContextCurrent()
{
    glfwMakeContextCurrent(_hwnd);
#if VE_PLATFORM == VE_PLATFORM_WIN32
    g_currentContext = _glContext;
    if (!_isInited) {
        if (glewInit() != GLEW_OK) {
            veLog("glewInit error!");
            return false;
        }
        _isInited = true;
    }
#endif
    
    return true;
}

void veViewerDesktop::swapBuffers()
{
    glfwSwapBuffers(_hwnd);
}

#if (VE_PLATFORM == VE_PLATFORM_WIN32)
void veViewerDesktop::startSimulation()
{
    if (!_sceneManager.valid()) return;
    veViewer::startSimulation();
    _isRunning = true;
    
    _updateThread = std::thread([this] {
        LARGE_INTEGER frequency;
        LARGE_INTEGER frameTimeLimit;
        LARGE_INTEGER preFrameTime;
        QueryPerformanceFrequency(&frequency);
        frameTimeLimit.QuadPart = (1.0 / 60.0) * frequency.QuadPart;
        QueryPerformanceCounter(&preFrameTime);
        double frequencyPreSec = 1.0 / frequency.QuadPart;
        while(_isRunning){
            
            LARGE_INTEGER currentFrameTime;
            QueryPerformanceCounter(&currentFrameTime);
            if (frameTimeLimit.QuadPart <= (currentFrameTime.QuadPart - preFrameTime.QuadPart)){
                this->update((currentFrameTime.QuadPart - preFrameTime.QuadPart) * frequencyPreSec);
                preFrameTime.QuadPart = currentFrameTime.QuadPart;
            }
        }
    });
    
    _renderingThread = std::thread([this] {
        while(_isRunning && _hwnd){
            _sceneManager->render(this);
        }
    });
}
#endif


#if (VE_PLATFORM == VE_PLATFORM_MAC)
void veViewerDesktop::startSimulation()
{
    if (!_sceneManager.valid()) return;
    veViewer::startSimulation();
    
    _isRunning = true;
    
    _updateThread = std::thread([this] {
        clock_t frameTimeClocks = 1.0 / 60.0 * CLOCKS_PER_SEC;
        clock_t preFrameTime = clock();
        double invertClocksSec = 1.0 / (double)CLOCKS_PER_SEC;
        while(_isRunning){
            clock_t currentFrameTime = clock();
            this->update((currentFrameTime - preFrameTime) * invertClocksSec);
            while ((clock() - currentFrameTime) < frameTimeClocks) {
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
            preFrameTime = currentFrameTime;
        }
    });
    
    _renderingThread = std::thread([this] {
        while(_isRunning && _hwnd){
            _sceneManager->render(this);
        }
    });
}
#endif

void veViewerDesktop::stopSimulation()
{
    if (!_sceneManager.valid()) return;
    if (!_isRunning) return;
    _isRunning = false;
    _updateThread.join();
    _renderingThread.join();
    veViewer::stopSimulation();
}

void veViewerDesktop::update(double deltaTime)
{
    _sceneManager->setDeltaTime(deltaTime);
    {
        std::unique_lock<std::mutex> lock(_eventMutex);
        _sceneManager->event(this);
        _eventList.clear();
    }
    _sceneManager->update(this);
}

void veViewerDesktop::render()
{
    _sceneManager->render(this);
}

void veViewerDesktop::create()
{
    if (_hwnd) return;
    _hwnd = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, _sharedViewer? _sharedViewer->_hwnd: nullptr);
    
    glfwSetWindowUserPointer(_hwnd, this);
    glfwSetKeyCallback(_hwnd, collectKeyEvent);
    glfwSetCharCallback(_hwnd, collectCharEvent);
    glfwSetMouseButtonCallback(_hwnd, collectMouseEvent);
    glfwSetCursorPosCallback(_hwnd, collectMouseMoveEvent);
    glfwSetScrollCallback(_hwnd, collectScrollEvent);
    glfwSetWindowSizeCallback(_hwnd, collectWindowSizeEvent);
    glfwSetWindowFocusCallback(_hwnd, collectWindowFocusEvent);
    glfwSetWindowCloseCallback(_hwnd, collectWindowClose);
    
    _currentEvent.setEventType(veEvent::VE_WIN_INIT);
    _currentEvent.setWindowWidth(_width);
    _currentEvent.setWindowHeight(_height);
    _eventList.push_back(_currentEvent);
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

void veViewerDesktop::caculateMouseUnitCoords(GLFWwindow* window, double x, double y)
{
    veViewerDesktop *viewer = static_cast<veViewerDesktop *>(glfwGetWindowUserPointer(window));
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    if (width == 0 || height == 0) return;
    double coordX = x / (double)width;
    double coordY = 1.0 - y / (double)height;
    viewer->_currentEvent.setMouseX((coordX - 0.5) * 2.0);
    viewer->_currentEvent.setMouseY((coordY - 0.5) * 2.0);
}

void veViewerDesktop::collectKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    veViewerDesktop *viewer = static_cast<veViewerDesktop *>(glfwGetWindowUserPointer(window));
    std::unique_lock<std::mutex> lock(viewer->_eventMutex);
    veEvent &event = viewer->_currentEvent;
    event.setKeySymbol(g_KeySymbolMap[key]);
    event.setEventType(action == GLFW_PRESS ? veEvent::VE_DOWN : veEvent::VE_UP);
    event.setModKeySymbol(g_ModKeySymbolMap[mods]);
    event.setKeyChar(0);
    viewer->_eventList.push_back(event);
}

void veViewerDesktop::collectCharEvent(GLFWwindow* window, unsigned int c)
{
    veViewerDesktop *viewer = static_cast<veViewerDesktop *>(glfwGetWindowUserPointer(window));
    std::unique_lock<std::mutex> lock(viewer->_eventMutex);
    veEvent &event = viewer->_eventList.back();
    event.setKeyChar(c);
}

void veViewerDesktop::collectMouseEvent(GLFWwindow* window, int button, int action, int mods)
{
    veViewerDesktop *viewer = static_cast<veViewerDesktop *>(glfwGetWindowUserPointer(window));
    std::unique_lock<std::mutex> lock(viewer->_eventMutex);
    veEvent &event = viewer->_currentEvent;
    event.setMouseSymbol(g_MouseSymbolMap[button]);
    event.setEventType(action == GLFW_PRESS ? veEvent::VE_PRESS : veEvent::VE_RELEASE);
    event.setModKeySymbol(g_ModKeySymbolMap[mods]);
    viewer->_eventList.push_back(event);
}

void veViewerDesktop::collectMouseMoveEvent(GLFWwindow* window, double x, double y)
{
    veViewerDesktop *viewer = static_cast<veViewerDesktop *>(glfwGetWindowUserPointer(window));
    std::unique_lock<std::mutex> lock(viewer->_eventMutex);
    caculateMouseUnitCoords(window, x, y);
    veEvent &event = viewer->_currentEvent;
    if (event.getEventType() != veEvent::VE_DRAG) {
        if (event.getEventType() == veEvent::VE_PRESS) {
            event.setEventType(veEvent::VE_DRAG);
        }
        else {
            event.setEventType(veEvent::VE_MOVE);
        }
    }
    viewer->_eventList.push_back(event);
}

void veViewerDesktop::collectScrollEvent(GLFWwindow* window, double x, double y)
{
    veViewerDesktop *viewer = static_cast<veViewerDesktop *>(glfwGetWindowUserPointer(window));
    std::unique_lock<std::mutex> lock(viewer->_eventMutex);
    veEvent &event = viewer->_currentEvent;
    event.setEventType(0.0 < y ? veEvent::VE_SCROLL_UP : veEvent::VE_SCROLL_DOWN);
    event.setMouseScroll(y);
    viewer->_eventList.push_back(event);
}

void veViewerDesktop::collectWindowSizeEvent(GLFWwindow* window, int width, int height)
{
    if (width == 0 || height == 0) return;
    veViewerDesktop *viewer = static_cast<veViewerDesktop *>(glfwGetWindowUserPointer(window));
    std::unique_lock<std::mutex> lock(viewer->_eventMutex);
    viewer->_width = width;
    viewer->_height = height;
    veEvent &event = viewer->_currentEvent;
    event.setEventType(veEvent::VE_WIN_RESIZE);
    event.setWindowWidth(width);
    event.setWindowHeight(height);
    if (viewer->getCamera())
        viewer->getCamera()->resize(event.getWindowWidth(), event.getWindowHeight());
    viewer->_sceneManager->destroyRenderContexts();
    viewer->_eventList.push_back(event);
}

void veViewerDesktop::collectWindowFocusEvent(GLFWwindow* window, int focused)
{
    veViewerDesktop *viewer = static_cast<veViewerDesktop *>(glfwGetWindowUserPointer(window));
    std::unique_lock<std::mutex> lock(viewer->_eventMutex);
    veEvent &event = viewer->_currentEvent;
    event.setEventType(focused != 0? veEvent::VE_WIN_FOCUS: veEvent::VE_WIN_NOFOCUS);
    viewer->_eventList.push_back(event);
}

void veViewerDesktop::collectWindowClose(GLFWwindow* window)
{
    veViewerDesktop *viewer = static_cast<veViewerDesktop *>(glfwGetWindowUserPointer(window));
    std::unique_lock<std::mutex> lock(viewer->_eventMutex);
    veEvent &event = viewer->_currentEvent;
    event.setEventType(veEvent::VE_WIN_CLOSE);
    viewer->_eventList.push_back(event);
}

#endif