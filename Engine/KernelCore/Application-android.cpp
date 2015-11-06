#include "Application-android.h"
#include "FileCore/File-android.h"
#include <android/input.h>

veApplicationAndroid::veApplicationAndroid()
    : _androidApp(nullptr)
    , _display(EGL_NO_DISPLAY)
    , _surface(EGL_NO_SURFACE)
    , _context(EGL_NO_CONTEXT){

}

veApplicationAndroid::~veApplicationAndroid() {
    eglDestroyContext(_display, _context);
    eglDestroySurface(_display, _surface);
}

bool veApplicationAndroid::makeContextCurrent() {
    //veLog("makeContextCurrent");
    if (_display && _surface && _context){
        if (eglMakeCurrent(_display, _surface, _surface, _context) == EGL_TRUE);
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
    if (_display == EGL_NO_DISPLAY){
        _display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (!_display) veLog("_display is null!");
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
        EGLint numConfigs;
        eglChooseConfig(_display, attribs, &_config, 1, &numConfigs);
        if (!numConfigs){
            veLog("eglChooseConfig failed!");
            return;
        }

        eglGetConfigAttrib(_display, _config, EGL_NATIVE_VISUAL_ID, &format);
        ANativeWindow_setBuffersGeometry(_androidApp->window, 0, 0, format);
        if (_display){
           veLog("init display");
        }
    }

    if (_surface == EGL_NO_SURFACE){
        _surface = eglCreateWindowSurface(_display, _config, _androidApp->window, NULL);
        eglQuerySurface( _display, _surface, EGL_WIDTH, &_width );
        eglQuerySurface( _display, _surface, EGL_HEIGHT, &_height );
        if (_surface){
           veLog("init surface");
        }
    }

    if (_context == EGL_NO_CONTEXT){
        const EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, //Request opengl ES3.0
                                           EGL_NONE };
        _context = eglCreateContext(_display, _config, NULL, context_attribs);
        if (_context){
           veLog("init context");
        }
    }
    veLog("initGLContext()");
}

void veApplicationAndroid::terminate() {
    //eglDestroyContext(_display, _context);
    eglDestroySurface(_display, _surface);
    //_context = EGL_NO_CONTEXT;
    _surface = EGL_NO_SURFACE;
    veLog("terminate()");
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
            event.setEventType(veEvent::VE_WIN_RESIZE);
            event.setWindowWidth(thisApp->width());
            event.setWindowHeight(thisApp->height());
        }
            break;
        case APP_CMD_TERM_WINDOW:
        {
            thisApp->terminate();
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