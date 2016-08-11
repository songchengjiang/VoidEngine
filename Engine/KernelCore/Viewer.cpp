#include "Viewer.h"
#include "SceneManager.h"

veViewer::veViewer(int width, int height, const std::string &title)
    : _width(width)
    , _height(height)
    , _title(title)
    , _isRunning(false)
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