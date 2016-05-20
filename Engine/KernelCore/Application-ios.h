#ifndef _VE_APPLICATION_IOS_
#define _VE_APPLICATION_IOS_
#include "Application.h"

#if (VE_PLATFORM == VE_PLATFORM_IOS)
class VE_EXPORT veApplicationIOS : public veApplication
{
    friend class veApplication;
public:
    virtual ~veApplicationIOS();

    virtual bool makeContextCurrent() override;
    virtual void swapBuffers() override;

    virtual bool run() override ;
    virtual void stop() override ;
    
    void simulationFrame(double deltaTime);
    
    void onTouchBegan(int touchID, veReal x, veReal y);
    void onTouchMove(int touchID, veReal x, veReal y);
    void onTouchEnd(int touchID, veReal x, veReal y);
    void onPushCurrentEvent();

private:

    veApplicationIOS();

    virtual void dispatchEvents() override;
    virtual void initWindowImplementation(void *param) override;
    virtual bool isWindowShouldClose() override;
    
private:
    
    void *_glView;
};
#endif

#endif //ANDROID_APPLICATION_ANDROID_H
