#include "Viewer.h"
#include "SceneManager.h"
#include <algorithm>

veViewer::veViewer(int width, int height, const std::string &title)
    : _width(width)
    , _height(height)
    , _title(title)
    , _camera(nullptr)
    , _isRunning(false)
    , _needDestroyRenderContexts(false)
{
    static unsigned int totalContextID = 0;
    _contextID = totalContextID;
    ++totalContextID;
}

veViewer::~veViewer()
{
}

void veViewer::startSimulation()
{
    _sceneManager->attachViewer(this);
}

void veViewer::stopSimulation()
{
    _sceneManager->detachViewer(this);
}

void veViewer::addListener(veViewerListener *listener)
{
    _listenerList.push_back(listener);
}

void veViewer::removeListener(veViewerListener *listener)
{
    auto iter = std::find(_listenerList.begin(), _listenerList.end(), listener);
    if (iter != _listenerList.end()) {
        _listenerList.erase(iter);
    }
}
