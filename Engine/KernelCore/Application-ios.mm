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
    
    for (auto &viewer : _viewerList){
        viewer->create();
        viewer->startSimulation();
    }
    return true;
}

void veApplicationIOS::stop() {
    for (auto &viewer : _viewerList){
        viewer->stopSimulation();
        viewer->destroy();
    }
}