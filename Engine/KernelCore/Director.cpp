#include "Director.h"
#include "OctreeSceneManager.h"
#include <time.h>

veDirector::veDirector()
	: _isRunning(false)
{
}

veDirector::~veDirector()
{
	stop();
}

veDirector* veDirector::instance()
{
	static veDirector director;
	return &director;
}

void veDirector::setSceneManager(veSceneManager *sm)
{
	if (_sceneManager.valid())
		_sceneManager->stopThreading();
	_sceneManager = sm;
}

bool veDirector::run()
{
	if (!_sceneManager.valid()) return false;
    _isRunning = true;
	_sceneManager->startThreading();
	double preFrameTime = (double)clock() / (double)CLOCKS_PER_SEC;
	while (_isRunning)
	{
		double currentFrameTime = (double)clock() / (double)CLOCKS_PER_SEC;
		_sceneManager->setDeltaTime(currentFrameTime - preFrameTime);
		_sceneManager->simulation();
		preFrameTime = currentFrameTime;
	}
	_sceneManager->stopThreading();
	return true;
}

void veDirector::stop()
{
	_isRunning = false;
}
