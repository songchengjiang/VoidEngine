#if (VE_PLATFORM == VE_PLATFORM_IOS)

#include "Viewer-ios.h"
#include "Application-ios.h"
#include "SceneManager.h"

#import "GLView.h"

veViewerIOS::veViewerIOS(int width, int height, const std::string &title, veViewerIOS *sharedViewer)
    : veViewer(width, height, title)
    , _sharedViewer(sharedViewer)
    , _glView(nullptr)
{
}

veViewerIOS::~veViewerIOS()
{
    
}

bool veViewerIOS::makeContextCurrent()
{
    veGLView *glView = (__bridge veGLView *)_glView;
    BOOL state = [glView makeContextCurrent];
    return state == YES? true: false;
}

void veViewerIOS::swapBuffers()
{
    veGLView *glView = (__bridge veGLView *)_glView;
    [glView swapBuffers];
}

void veViewerIOS::startSimulation()
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
 
    veGLView *glView = (__bridge veGLView *)_glView;
    [glView startRendering];
}

void veViewerIOS::stopSimulation()
{
    if (!_sceneManager.valid()) return;
    if (!_isRunning) return;
    _isRunning = false;
    _updateThread.join();
    
    veGLView *glView = (__bridge veGLView *)_glView;
    [glView stopRendering];
}

void veViewerIOS::update(double deltaTime)
{
    _sceneManager->setDeltaTime(deltaTime);
    {
        std::unique_lock<std::mutex> lock(_eventMutex);
        _sceneManager->event(this);
        _eventList.clear();
    }
    _sceneManager->update(this);
}

void veViewerIOS::create()
{
    _currentEvent.setEventType(veEvent::VE_WIN_INIT);
    _currentEvent.setWindowWidth(_width);
    _currentEvent.setWindowHeight(_height);
    _eventList.push_back(_currentEvent);
}

void veViewerIOS::destroy()
{
    if (!_glView) return;
    _glView = nullptr;
}

void veViewerIOS::show()
{
}

void veViewerIOS::hide()
{
}

void veViewerIOS::onTouchBegan(int touchID, veReal x, veReal y)
{
    veEvent &event = _currentEvent;
    event.addTouch({touchID, x, y, x, y});
    event.setEventType(veEvent::VE_TOUCH_START);
}

void veViewerIOS::onTouchMove(int touchID, veReal x, veReal y)
{
    veEvent &event = _currentEvent;
    for (auto &touch : event.getTouches()){
        if (touch.id == touchID){
            touch.latestx = touch.x;
            touch.latesty = touch.y;
            touch.x = x;
            touch.y = y;
            event.setEventType(veEvent::VE_TOUCH_MOVE);
            break;
        }
    }
}

void veViewerIOS::onTouchEnd(int touchID, veReal x, veReal y)
{
    veEvent &event = _currentEvent;
    event.removeTouch(touchID);
    event.setEventType(veEvent::VE_TOUCH_END);
}

void veViewerIOS::onPushCurrentEvent()
{
    std::unique_lock<std::mutex> lock(_eventMutex);
    _eventList.push_back(_currentEvent);
}

#endif