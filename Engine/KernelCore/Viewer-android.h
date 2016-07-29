#ifndef _VE_VIEWER_ANDROID_
#define _VE_VIEWER_ANDROID_

#include "Viewer.h"
#include <thread>
#include <mutex>

class VE_EXPORT veViewerAndroid : public veViewer
{
    friend class veApplicationAndroid;
public:

    virtual ~veViewerAndroid();

    virtual bool makeContextCurrent() override;
    virtual void swapBuffers() override;
    virtual void create() override;
    virtual void destroy() override;
    virtual void show() override;
    virtual void hide() override;

    virtual bool simulation(double deltaTime) override;
    virtual void startRender() override;
    virtual void stopRender() override;
    void frameRender();
    void resize(int width, int height);

    void onTouchBegan(int touchID, veReal x, veReal y);
    void onTouchEnd(int touchID, veReal x, veReal y);
    void onTouchsMove(int n, int touchIDs[], veReal xs[], veReal ys[]);

protected:

    veViewerAndroid(int width, int height, const std::string &title, veViewerAndroid *sharedViewer);

protected:

    veViewerAndroid *_sharedViewer;
    std::mutex  _eventMutex;
};

#endif
