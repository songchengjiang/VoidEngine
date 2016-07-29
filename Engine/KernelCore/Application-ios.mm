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
    auto viewer = new veViewerIOS(width, height, title, static_cast<veViewerIOS *>(sharedContextViewer));
    _viewerList.push_back(viewer);
    return viewer;
}

bool veApplicationIOS::run() {
    if (_viewerList.empty()) return false;
    
    _isRunning = true;
    for (auto &viewer : _viewerList){
        viewer->create();
        viewer->startRender();
    }
    _runningThread = std::thread([this] {
        clock_t frameTimeClocks = 1.0 / 60.0 * CLOCKS_PER_SEC;
        clock_t preFrameTime = clock();
        double invertClocksSec = 1.0 / (double)CLOCKS_PER_SEC;
        while (_isRunning){
            clock_t currentFrameTime = clock();
            for (auto &viewer : _viewerList){
                viewer->simulation((currentFrameTime - preFrameTime) * invertClocksSec);
            }
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
    for (auto &viewer : _viewerList){
        viewer->stopRender();
        viewer->destroy();
    }
    _runningThread.join();
}