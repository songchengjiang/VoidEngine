#ifndef _VE_APPLICATION_ANDROID_
#define _VE_APPLICATION_ANDROID_
#include "Application.h"
#include <mutex>
#include <condition_variable>
#include <jni.h>

class VE_EXPORT veApplicationAndroid : public veApplication
{
    friend class veApplication;
public:
    virtual ~veApplicationAndroid();

    virtual veViewer* createViewer(int width, int height, const std::string &title, veViewer *sharedContextViewer = nullptr) override;

    virtual bool run() override;
    virtual void stop() override;

    void setJavaVM(JavaVM *vm) { _javaVM = vm; }
    JavaVM* getJavaVM() const { return _javaVM; }

private:

    veApplicationAndroid();

private:

    JavaVM *_javaVM;

    //void pollAllEvents();

//    static void collectWindowEvents(struct android_app* app, int32_t cmd);
//    static int32_t collectInputEvents(struct android_app* app, AInputEvent* event);
};

#endif //ANDROID_APPLICATION_ANDROID_H
