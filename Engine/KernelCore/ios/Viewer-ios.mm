#if (VE_PLATFORM == VE_PLATFORM_IOS)

#include "Viewer-ios.h"
#include "Application-ios.h"
#include "SceneManager.h"

#import "GLView.h"

veViewerIOS::veViewerIOS(int width, int height, const std::string &title, veViewerIOS *sharedViewer)
    : veViewer(width, height, title)
    , _sharedViewer(sharedViewer)
    , _glView(nullptr)
    , _isRendering(false)
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

bool veViewerIOS::simulation(double deltaTime)
{
    if (!_sceneManager.valid()) return false;
    _sceneManager->setDeltaTime(deltaTime);
    _sceneManager->event(this);
    _eventList.clear();
    bool needUpdateSceneManager = _sharedViewer? _sharedViewer->_sceneManager != _sceneManager: true;
    if (needUpdateSceneManager)
        _sceneManager->update();
    return true;
}

void veViewerIOS::startRender()
{
    veGLView *glView = (__bridge veGLView *)_glView;
    [glView startRendering];
}

void veViewerIOS::stopRender()
{
    veGLView *glView = (__bridge veGLView *)_glView;
    [glView startRendering];
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
    _eventList.push_back(_currentEvent);
}

#endif