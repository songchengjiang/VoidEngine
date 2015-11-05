#include "Application-android.h"
#include "FileCore/File-android.h"
#include <android/input.h>

veApplicationAndroid::veApplicationAndroid()
    : _androidApp(nullptr)
    , _display(nullptr)
    , _surface(nullptr)
    , _context(nullptr){

}

veApplicationAndroid::~veApplicationAndroid() {

}

bool veApplicationAndroid::makeContextCurrent() {
    //veLog("makeContextCurrent");
    if (_display && _surface && _context){
        eglMakeCurrent(_display, _surface, _surface, _context);
        return true;
    }
    return false;
}

void veApplicationAndroid::swapBuffers() {
    //veLog("swapBuffers");
    if (_display && _surface)
        eglSwapBuffers(_display, _surface);
}

void veApplicationAndroid::dispatchEvents() {
    pollAllEvents();
    for (auto &event : _events) {
        _sceneManager->dispatchEvents(event);
    }
    _events.clear();
}

void veApplicationAndroid::initWindowImplementation(void *param) {
    _androidApp = static_cast<android_app* >(param);
    _androidApp->userData = this;
    _androidApp->onAppCmd = veApplicationAndroid::collectWindowEvents;
    _androidApp->onInputEvent = veApplicationAndroid::collectInputEvents;
    veFileAndroid::ASSET_MANAGER = _androidApp->activity->assetManager;
}

bool veApplicationAndroid::isWindowShouldClose() {
    return  _androidApp->destroyRequested != 0;
}

bool veApplicationAndroid::run()
{
    if (!_sceneManager.valid()) return false;
    _isRunning = true;
    _sceneManager->startThreading();
    veLog("startThreading!!!!!!!");
    double preFrameTime = (double)clock() / (double)CLOCKS_PER_SEC;
    while (_isRunning && !isWindowShouldClose())
    {
        double currentFrameTime = (double)clock() / (double)CLOCKS_PER_SEC;
        _sceneManager->setDeltaTime(currentFrameTime - preFrameTime);
        this->dispatchEvents();
        _sceneManager->simulation();
        preFrameTime = currentFrameTime;
    }
    _sceneManager->stopThreading();
    return true;
}

void veApplicationAndroid::pollAllEvents() {
    int ident = 0;
    int events = 0;
    struct android_poll_source* source = nullptr;
    while ((ident=ALooper_pollAll(_isRunning ? 0 : -1, NULL, &events,
                                  (void**)&source)) >= 0) {
        // Process this event.
        if (source != NULL) {
            source->process(_androidApp, source);
        }
    }
}

void veApplicationAndroid::initGLContext() {
    _display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (!_display) veLog("_display null");
    eglInitialize(_display, nullptr, nullptr);
    EGLint attribs[] = {
            EGL_RED_SIZE,       8,
            EGL_GREEN_SIZE,     8,
            EGL_BLUE_SIZE,      8,
            EGL_ALPHA_SIZE,     8,
            EGL_DEPTH_SIZE,     24,
           // EGL_STENCIL_SIZE,   0,
            //EGL_SAMPLE_BUFFERS, 0,
            //EGL_LEVEL,            0,
            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE,EGL_OPENGL_ES2_BIT,
            EGL_NONE
    };

    EGLint w, h, dummy, format;
    EGLConfig config;
    EGLint numConfigs;
    eglChooseConfig(_display, attribs, &config, 1, &numConfigs);
    eglGetConfigAttrib(_display, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(_androidApp->window, _width, _height, format);
    _surface = eglCreateWindowSurface(_display, config, _androidApp->window, NULL);
    _context = eglCreateContext(_display, config, NULL, NULL);

    if (!_surface) veLog("_surface null");
    if (!_context) veLog("_context null");
    veLog("initGLContext Done!!!");
}

void veApplicationAndroid::collectWindowEvents(struct android_app *app, int32_t cmd) {
    veApplicationAndroid* thisApp = static_cast<veApplicationAndroid *>(app->userData);
    veEvent &event = thisApp->_currentEvent;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            break;
        case APP_CMD_INIT_WINDOW:
        {
            thisApp->initGLContext();
        }
            break;
        case APP_CMD_TERM_WINDOW:
        {
            event.setEventType(veEvent::VE_WIN_CLOSE);
        }
            break;
        case APP_CMD_GAINED_FOCUS:
        {
            event.setEventType(veEvent::VE_WIN_FOCUS);
        }
            break;
        case APP_CMD_LOST_FOCUS:
        {
            event.setEventType(veEvent::VE_WIN_NOFOCUS);
        }
            break;
    }
    thisApp->_events.push_back(event);
}

int32_t veApplicationAndroid::collectInputEvents(struct android_app *app, AInputEvent *event) {
    veApplicationAndroid* thisApp = static_cast<veApplicationAndroid *>(app->userData);
    veEvent &veEvent = thisApp->_currentEvent;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION){

    }else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY){

    }
    thisApp->_events.push_back(veEvent);
}