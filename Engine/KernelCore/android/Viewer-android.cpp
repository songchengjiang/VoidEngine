#include "Viewer-android.h"
#include "KernelCore/SceneManager.h"

veViewerAndroid::veViewerAndroid(int width, int height, const std::string &title, veViewerAndroid *sharedViewer)
    : veViewer(width, height, title)
    , _sharedViewer(sharedViewer)
{
    
}

veViewerAndroid::~veViewerAndroid()
{
}

bool veViewerAndroid::makeContextCurrent()
{
    return true;
}

void veViewerAndroid::swapBuffers()
{
}

void veViewerAndroid::startSimulation()
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
}

void veViewerAndroid::stopSimulation()
{
    if (!_sceneManager.valid()) return;
    if (!_isRunning) return;
    _isRunning = false;
    _updateThread.join();
    veViewer::stopSimulation();
}

void veViewerAndroid::update(double deltaTime)
{
    _sceneManager->setDeltaTime(deltaTime);
    {
        std::unique_lock<std::mutex> lock(_eventMutex);
        _sceneManager->event(this);
        _eventList.clear();
    }
    _sceneManager->update(this);
}

void veViewerAndroid::render()
{
    if (!_sceneManager.valid()) return;
    _sceneManager->render(this);
}

void veViewerAndroid::create()
{
    _currentEvent.setEventType(veEvent::VE_WIN_INIT);
    _currentEvent.setWindowWidth(_width);
    _currentEvent.setWindowHeight(_height);
    _eventList.push_back(_currentEvent);
}

void veViewerAndroid::destroy()
{
}

void veViewerAndroid::show()
{
}

void veViewerAndroid::hide()
{

}

void veViewerAndroid::resize(int width, int height)
{
    _width = width;
    _height = height;
    _currentEvent.setEventType(veEvent::VE_WIN_RESIZE);
    _currentEvent.setWindowWidth(width);
    _currentEvent.setWindowHeight(height);
    if (this->getCamera())
        this->getCamera()->resize(_currentEvent.getWindowWidth(), _currentEvent.getWindowHeight());
    this->_eventList.push_back(_currentEvent);
}

void veViewerAndroid::onTouchBegan(int touchID, veReal x, veReal y)
{
    std::unique_lock<std::mutex> eventLock(_eventMutex);
    x = x / _width;
    y = 1.0f - y / _height;
    x = (x - 0.5f) * 2.0f;
    y = (y - 0.5f) * 2.0f;
    veEvent &event = _currentEvent;
    event.addTouch({touchID, x, y, x, y});
    event.setEventType(veEvent::VE_TOUCH_START);
    this->_eventList.push_back(_currentEvent);
}

void veViewerAndroid::onTouchEnd(int touchID, veReal x, veReal y)
{
    std::unique_lock<std::mutex> eventLock(_eventMutex);
    veEvent &event = _currentEvent;
    event.removeTouch(touchID);
    event.setEventType(veEvent::VE_TOUCH_END);
    this->_eventList.push_back(_currentEvent);
}

void veViewerAndroid::onTouchsMove(int n, int touchIDs[], veReal xs[], veReal ys[])
{
    std::unique_lock<std::mutex> eventLock(_eventMutex);
    veEvent &event = _currentEvent;
    for (int i = 0; i < n; ++i) {
        int touchID = touchIDs[i];
        veReal x = xs[i];
        veReal y = ys[i];
        x = x / _width;
        y = 1.0f - y / _height;
        x = (x - 0.5f) * 2.0f;
        y = (y - 0.5f) * 2.0f;
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
    this->_eventList.push_back(_currentEvent);
}
