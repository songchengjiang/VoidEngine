#ifndef _VE_VIEWER_IOS_
#define _VE_VIEWER_IOS_
#include "KernelCore/Viewer.h"
#include <thread>
#include <mutex>
#include <condition_variable>

#if (VE_PLATFORM == VE_PLATFORM_IOS)

class veSceneManager;
class VE_EXPORT veViewerIOS : public veViewer
{
    friend class veApplicationIOS;
public:
    virtual ~veViewerIOS();
    
    virtual bool makeContextCurrent() override;
    virtual void swapBuffers() override;
    virtual void create() override;
    virtual void destroy() override;
    virtual void show() override;
    virtual void hide() override;
    
    
    virtual void startSimulation() override;
    virtual void stopSimulation() override;
    
    void onTouchBegan(int touchID, veReal x, veReal y);
    void onTouchMove(int touchID, veReal x, veReal y);
    void onTouchEnd(int touchID, veReal x, veReal y);
    void onPushCurrentEvent();
    
    void setGLView(void *glView) { _glView = glView; }
    void* getGLView() { return _glView; }
    
protected:
    
    veViewerIOS(int width, int height, const std::string &title, veViewerIOS *sharedViewer = nullptr);
    void update(double deltaTime);
    
protected:
    
    void        *_glView;
    veViewerIOS *_sharedViewer;
    
    std::thread _updateThread;
    std::mutex  _eventMutex;
};

#endif

#endif
