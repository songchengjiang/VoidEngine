#ifndef _VE_VIEWER_
#define _VE_VIEWER_
#include "Prerequisites.h"
#include "Event.h"
#include "Camera.h"
#include "SceneManager.h"

class VE_EXPORT veViewer
{
public:
    veViewer(int width, int height, const std::string &title);
    virtual ~veViewer();
    
    virtual bool makeContextCurrent() = 0;
    virtual void swapBuffers() = 0;
    virtual void create() = 0;
    virtual void destroy() = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    
    virtual bool simulation(double deltaTime) = 0;
    virtual void startRender() = 0;
    virtual void stopRender() = 0;
    
    int width() const { return _width; }
    int height() const { return _height; }
    const std::string& title() const { return _title; }
    
    const veEventList& getEventList() const { return _eventList; }
    
    void setCamera(veCamera *camera) { _camera = camera; }
    veCamera* getCamera() { return _camera; }

    void setSceneManager(veSceneManager *sm) { _sceneManager = sm; }
    veSceneManager* getSceneManager() { return _sceneManager.get(); }
    
    unsigned int getContextID() const { return _contextID; }
    
protected:
    
    int                    _width;
    int                    _height;
    std::string            _title;
    veCamera              *_camera;
    VE_Ptr<veSceneManager> _sceneManager;
    veEventList            _eventList;
    veEvent                _currentEvent;
    
    bool                   _isRunning;
    unsigned int           _contextID;
};

#endif
