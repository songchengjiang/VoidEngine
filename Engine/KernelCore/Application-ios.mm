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

#endif