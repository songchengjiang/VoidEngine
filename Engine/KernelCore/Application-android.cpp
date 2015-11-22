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
    clock_t frameTimeClocks = 1.0 / 60.0 * CLOCKS_PER_SEC;
    clock_t preFrameTime = clock();
    double invertClocksSec = 1.0 / (double)CLOCKS_PER_SEC;
    _sceneManager->startThreading();
    while (_isRunning && !isWindowShouldClose())
    {
        clock_t currentFrameTime = clock();
        _sceneManager->setDeltaTime((currentFrameTime - preFrameTime) * invertClocksSec);
        veLog("FRAME RATE: %f\n", 1.0 / _sceneManager->getDeltaTime());
        this->dispatchEvents();
        _sceneManager->simulation();
        while ((clock() - currentFrameTime) < frameTimeClocks) {
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
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

int32_t veApplicationAndroid::collectInputEvents(struct android_app *app, AInputEvent *aEvent) {
    veApplicationAndroid* thisApp = static_cast<veApplicationAndroid *>(app->userData);
    veEvent &event = thisApp->_currentEvent;
    bool state = false;
    if (AInputEvent_getType(aEvent) == AINPUT_EVENT_TYPE_MOTION){
        int32_t action = AMotionEvent_getAction( aEvent );
        int32_t index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                        >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        uint32_t flags = action & AMOTION_EVENT_ACTION_MASK;
        int32_t count = AMotionEvent_getPointerCount( aEvent );

        switch( flags )
        {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_UP:
            {
                int32_t idx = (flags == AMOTION_EVENT_ACTION_DOWN || flags == AMOTION_EVENT_ACTION_UP)? 0: index;
                int32_t id = AMotionEvent_getPointerId(aEvent, idx);
                if (flags == AMOTION_EVENT_ACTION_DOWN || flags == AMOTION_EVENT_ACTION_POINTER_DOWN){
                    veReal x = (AMotionEvent_getX(aEvent, idx) / veReal(thisApp->width()));
                    veReal y = 1.0 - AMotionEvent_getY(aEvent, idx) / veReal(thisApp->height());
                    x = (x - 0.5) * 2.0;
                    y = (y - 0.5) * 2.0;
                    event.addTouch({id, x, y, x, y});
                    event.setEventType(veEvent::VE_TOUCH_START);
                   //veLog("VE_TOUCH_START(%d): (%f, %f)", id, x, y);
                }else{
                    event.removeTouch(id);
                    event.setEventType(veEvent::VE_TOUCH_END);
                    //veLog("VE_TOUCH_END");
                }
            }
                break;
            case AMOTION_EVENT_ACTION_MOVE:
            {
                for (int32_t i = 0; i < count; ++i){
                    veReal x = (AMotionEvent_getX(aEvent, i) / veReal(thisApp->width()));
                    veReal y = 1.0 - AMotionEvent_getY(aEvent, i) / veReal(thisApp->height());
                    x = (x - 0.5) * 2.0;
                    y = (y - 0.5) * 2.0;
                    int32_t id = AMotionEvent_getPointerId(aEvent, i);
                    auto &touches = event.getTouches();
                    for (auto iter = touches.begin(); iter != touches.end(); ++iter) {
                        if (iter->id == id) {
                            iter->latestx = iter->x;
                            iter->latesty = iter->y;
                            iter->x = x;
                            iter->y = y;
                            //veLog("VE_TOUCH_MOVE(%d): (%f, %f, %f, %f)", id, iter->latestx, iter->latesty, iter->x, iter->y);
                        }
                    }
                }
                if (0 < count){
                    event.setEventType(veEvent::VE_TOUCH_MOVE);
                }
            }
                break;
            case AMOTION_EVENT_ACTION_CANCEL:
                break;
        }
        state = true;
    }else if (AInputEvent_getType(aEvent) == AINPUT_EVENT_TYPE_KEY){
        state = true;
    }
    if (state){
        thisApp->_events.push_back(event);
        return 1;
    }
    return 0;
}