#ifndef _VE_VIEWER_ANDROID_
#define _VE_VIEWER_ANDROID_

#include "Viewer.h"
#include <thread>

class VE_EXPORT veViewerAndroid : public veViewer
{
public:
    veViewerAndroid(int width, int height, const std::string &title, veViewerAndroid *sharedViewer);
    virtual ~veViewerAndroid();
    
    virtual bool makeContextCurrent();
    virtual void swapBuffers();
    virtual void startRender(veSceneManager *sm);
    virtual void stopRender(veSceneManager *sm);
    virtual void create();
    virtual void destroy();
    virtual void show();
    virtual void hide();
    virtual bool isNeedClosed() const { return false; }

    void setNativeWindow(ANativeWindow *wnd) { _window = wnd; }

protected:

    EGLDisplay _display;
    EGLSurface _surface;
    EGLContext _context;
    EGLConfig  _config;
    ANativeWindow* _window;
    veViewerAndroid *_sharedViewer;

    bool        _isRendering;
    std::thread _renderingThread;
};

#endif
