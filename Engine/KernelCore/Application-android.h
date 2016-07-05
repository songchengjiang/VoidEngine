#ifndef _VE_APPLICATION_ANDROID_
#define _VE_APPLICATION_ANDROID_
#include "Application.h"
#include "android_native_app_glue.h"
#include <mutex>
#include <condition_variable>

class VE_EXPORT veApplicationAndroid : public veApplication
{
    friend class veApplication;
public:
    virtual ~veApplicationAndroid();

    virtual veViewer* createViewer(int width, int height, const std::string &title, veViewer *sharedContextViewer = nullptr) override;

    void setAndroidApp(android_app *app);
    virtual bool run() override;
    virtual void stop() override;

private:

    veApplicationAndroid();

    void pollAllEvents();
    void init();
    void terminate();

    static void collectWindowEvents(struct android_app* app, int32_t cmd);
    static int32_t collectInputEvents(struct android_app* app, AInputEvent* event);

private:

    veEvent      _currentEvent;
    android_app* _androidApp;
    bool         _isRunning;
};

#endif //ANDROID_APPLICATION_ANDROID_H
