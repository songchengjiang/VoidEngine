#include "Application-ios.h"
#import "ios/GLView.h"
#include "ios/Viewer-ios.h"

veApplicationIOS::veApplicationIOS(){

}

veApplicationIOS::~veApplicationIOS() {
    for (auto &viewer : _viewerList){
        viewer->destroy();
        VE_SAFE_DELETE(viewer);
    }
}

veViewer* veApplicationIOS::createViewer(int width, int height, const std::string &title, veViewer *sharedContextViewer)
{
    auto viewer = new veViewerIOS(width, height, title, sharedContextViewer);
    _viewerList.push_back(viewer);
    _viewerList.push_back(viewer);
    _currentEvent.setEventType(veEvent::VE_WIN_INIT);
    _currentEvent.setWindowWidth(width);
    _currentEvent.setWindowHeight(height);
    _eventDispatcher.addEvent(viewer, _currentEvent);
    return viewer;
}

bool veApplicationIOS::run() {
//    if (!_sceneManager.valid()) return false;
//    _sceneManager->startThreading();
//    veGLView *glView = (__bridge veGLView *)_glView;
//    [glView startRendering];
    
    if (!_sceneManager.valid()) return false;
    _isRunning = true;
    _runningThread = std::thread([this] {
        clock_t frameTimeClocks = 1.0 / 60.0 * CLOCKS_PER_SEC;
        clock_t preFrameTime = clock();
        double invertClocksSec = 1.0 / (double)CLOCKS_PER_SEC;
        while (_isRunning){
            std::unique_lock<std::mutex> lock(_eventMutex);
            clock_t currentFrameTime = clock();
            _sceneManager->update((currentFrameTime - preFrameTime) * invertClocksSec, &_eventDispatcher);
            _eventDispatcher.clearEventList();
            while ((clock() - currentFrameTime) < frameTimeClocks) {
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
            preFrameTime = currentFrameTime;
        }
    });
    for (auto &viewer : _viewerList){
        viewer->startRender(_sceneManager.get());
    }
    return true;
}

void veApplicationIOS::stop() {
    _isRunning = false;
    for (auto &viewer : _viewerList){
        viewer->stopRender(_sceneManager.get());
    }
    _runningThread.join();
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

void veApplicationIOS::onPushCurrentEvent(veViewer *viewer)
{
    std::unique_lock<std::mutex> lock(_eventMutex);
    _eventDispatcher.addEvent(viewer, _currentEvent);
}