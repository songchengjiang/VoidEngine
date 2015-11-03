#ifndef _VE_APPLICATION_ANDROID_
#define _VE_APPLICATION_ANDROID_
#include "Application.h"
#include "android_native_app_glue.h"

class VE_EXPORT veApplicationAndroid : public veApplication
{
    friend class veApplication;
public:
    virtual ~veApplicationAndroid();

    virtual void makeContextCurrent() override;
    virtual void swapBuffers() override;

private:

    veApplicationAndroid();

    virtual void dispatchEvents() override;
    virtual void initWindowImplementation(void *param) override;
    virtual bool isWindowShouldClose() override;

    void pollAllEvents();
    void initGLContext();

    static void collectWindowEvents(struct android_app* app, int32_t cmd);
    static int32_t collectInputEvents(struct android_app* app, AInputEvent* event);

private:

    android_app* _androidApp;

    EGLDisplay _display;
    EGLSurface _surface;
    EGLContext _context;
};

#endif //ANDROID_APPLICATION_ANDROID_H
