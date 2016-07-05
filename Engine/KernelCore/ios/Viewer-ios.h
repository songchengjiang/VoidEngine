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
    virtual void startRender(veSceneManager *sm) override;
    virtual void stopRender(veSceneManager *sm) override;
    virtual void create() override;
    virtual void destroy() override;
    virtual void show() override;
    virtual void hide() override;
    virtual bool isNeedClosed() const override;
    
    void createWithGLView(void *view);
    void* getGLView() { return _glView; }
    
protected:
    
    veViewerIOS(int width, int height, const std::string &title, void *sharedView);
    
protected:
    
    void       *_glView;
    void       *_sharedGLView;
    
    bool        _isRendering;
    std::thread _renderingThread;
};

#endif

#endif
