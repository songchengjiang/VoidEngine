#ifndef _VE_VIEWER_DESKTOP_
#define _VE_VIEWER_DESKTOP_
#include "Viewer.h"
#include <thread>
#include <mutex>
#include <condition_variable>

#if (VE_PLATFORM == VE_PLATFORM_WIN32) || (VE_PLATFORM == VE_PLATFORM_MAC)

class veSceneManager;
class VE_EXPORT veViewerDesktop : public veViewer
{
    friend class veApplicationDesktop;
public:
    virtual ~veViewerDesktop();
    
    virtual bool makeContextCurrent() override;
    virtual void swapBuffers() override;
    virtual void startRender(veSceneManager *sm) override;
    virtual void stopRender(veSceneManager *sm) override;
    virtual void create() override;
    virtual void destroy() override;
    virtual void show() override;
    virtual void hide() override;
    virtual bool isNeedClosed() const override;
    
    GLFWwindow* getHWND() { return _hwnd; }
    

protected:
    
    veViewerDesktop(int width, int height, const std::string &title, GLFWwindow *sharedHwnd = nullptr);
    
protected:
    
    GLFWwindow *_hwnd;
    GLFWwindow *_sharedHwnd;
    bool        _isInited;
    
    bool        _isRendering;
    std::thread _renderingThread;
};

#endif

#endif
