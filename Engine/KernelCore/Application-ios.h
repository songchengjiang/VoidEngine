#ifndef _VE_APPLICATION_IOS_
#define _VE_APPLICATION_IOS_
#include "Application.h"

#if (VE_PLATFORM == VE_PLATFORM_IOS)
class VE_EXPORT veApplicationIOS : public veApplication
{
    friend class veApplication;
public:
    virtual ~veApplicationIOS();

    virtual veViewer* createViewer(int width, int height, const std::string &title, veViewer *sharedContextViewer = nullptr)  override;
    
    virtual bool run() override ;
    virtual void stop() override ;
    
    void simulationFrame(double deltaTime);
    
    void onTouchBegan(int touchID, veReal x, veReal y);
    void onTouchMove(int touchID, veReal x, veReal y);
    void onTouchEnd(int touchID, veReal x, veReal y);
    void onPushCurrentEvent(veViewer *viewer);

private:

    veApplicationIOS();
    
private:
    
    void *_glView;
    std::thread _runningThread;
    std::mutex  _eventMutex;
    veEvent     _currentEvent;
    bool        _isRunning;
};
#endif

#endif //ANDROID_APPLICATION_ANDROID_H
