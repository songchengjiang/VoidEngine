#ifndef _VE_VIEWER_
#define _VE_VIEWER_
#include "Prerequisites.h"
#include "Event.h"
#include "Camera.h"

class veSceneManager;
class VE_EXPORT veViewer
{
public:
    veViewer(int width, int height, const std::string &title);
    virtual ~veViewer();
    
    virtual bool makeContextCurrent() = 0;
    virtual void swapBuffers() = 0;
    virtual void startRender(veSceneManager *sm) = 0;
    virtual void stopRender(veSceneManager *sm) = 0;
    virtual void create() = 0;
    virtual void destroy() = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual bool isNeedClosed() const = 0;
    
    int width() const { return _width; }
    int height() const { return _height; }
    const std::string& title() const { return _title; }
    
    void setCamera(veCamera *camera) { _camera = camera; }
    veCamera* getCamera() { return _camera.get(); }

    
protected:
    
    int                    _width;
    int                    _height;
    std::string            _title;
    VE_Ptr<veCamera>       _camera;
};

#endif
