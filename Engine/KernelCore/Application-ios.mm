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
    std::unique_lock<std::mutex> lock(_eventMutex);
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
//    if (!_sceneManager.valid()) return false;
//    _sceneManager->startThreading();
//    veGLView *glView = (__bridge veGLView *)_glView;
//    [glView startRendering];
    
    if (!_sceneManager.valid()) return false;
    _isRunning = true;
    _sceneManager->startThreading();
    _runningThread = std::thread([this] {
        clock_t frameTimeClocks = 1.0 / 60.0 * CLOCKS_PER_SEC;
        clock_t preFrameTime = clock();
        double invertClocksSec = 1.0 / (double)CLOCKS_PER_SEC;
        while (_isRunning && !isWindowShouldClose()){
            clock_t currentFrameTime = clock();
            _sceneManager->setDeltaTime((currentFrameTime - preFrameTime) * invertClocksSec);
            //veLog("Frame Rate: %f\n", 1.0 / _sceneManager->getDeltaTime());
            this->dispatchEvents();
            _sceneManager->simulation();
            while ((clock() - currentFrameTime) < frameTimeClocks) {
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
            preFrameTime = currentFrameTime;
        }
    });
    return true;
}

void veApplicationIOS::stop() {
    _isRunning = false;
    _runningThread.join();
    _sceneManager->stopThreading();
//    veGLView *glView = (__bridge veGLView *)_glView;
//    [glView stopRendering];
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
    std::unique_lock<std::mutex> lock(_eventMutex);
    _events.push_back(_currentEvent);
}

#endif