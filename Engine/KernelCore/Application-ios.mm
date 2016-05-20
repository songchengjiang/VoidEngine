#include "Application-ios.h"

#if (VE_PLATFORM == VE_PLATFORM_IOS)
#import "ios/GLView.h"

veApplicationIOS::veApplicationIOS(){

}

veApplicationIOS::~veApplicationIOS() {
}

bool veApplicationIOS::makeContextCurrent() {
    veGLView *glView = (__bridge veGLView *)_glView;
    BOOL state = [glView makeContextCurrent];
    return state == YES? true: false;
}

void veApplicationIOS::swapBuffers() {
    veGLView *glView = (__bridge veGLView *)_glView;
    [glView swapBuffers];
}

void veApplicationIOS::dispatchEvents() {
    for (auto &event : _events) {
        _sceneManager->dispatchEvents(event);
    }
    _events.clear();
}

void veApplicationIOS::initWindowImplementation(void *param) {
    _glView = param;
    veGLView *glView = (__bridge veGLView *)_glView;
    glView.application = this;
}

bool veApplicationIOS::isWindowShouldClose() {
    return false;
}

void veApplicationIOS::simulationFrame(double deltaTime) {
    _sceneManager->setDeltaTime(deltaTime);
    this->dispatchEvents();
    _sceneManager->simulation();
}

bool veApplicationIOS::run() {
    if (!_sceneManager.valid()) return false;
    _sceneManager->startThreading();
    veGLView *glView = (__bridge veGLView *)_glView;
    [glView startRendering];
    return true;
}

void veApplicationIOS::stop() {
    _sceneManager->stopThreading();
    veGLView *glView = (__bridge veGLView *)_glView;
    [glView stopRendering];
}

void veApplicationIOS::onTouchBegan(int touchID, veReal x, veReal y)
{
    veEvent &event = _currentEvent;
    event.addTouch({touchID, x, y, x, y});
    event.setEventType(veEvent::VE_TOUCH_START);
}

void veApplicationIOS::onTouchMove(int touchID, veReal x, veReal y)
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

void veApplicationIOS::onTouchEnd(int touchID, veReal x, veReal y)
{
    veEvent &event = _currentEvent;
    event.removeTouch(touchID);
    event.setEventType(veEvent::VE_TOUCH_END);
}

void veApplicationIOS::onPushCurrentEvent()
{
    _events.push_back(_currentEvent);
}

#endif