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
	startThreading();
	double preFrameTime = (double)clock() / (double)CLOCKS_PER_SEC;
	while (_isRunning)
	{
		double currentFrameTime = (double)clock() / (double)CLOCKS_PER_SEC;
		frame(currentFrameTime - preFrameTime);
		preFrameTime = currentFrameTime;
	}
	stopThreading();
	return true;
}

void veDirector::stop()
{
	_isRunning = false;
}

void veDirector::startThreading()
{
	if (_sceneManager.valid())
		_sceneManager->startThreading();
}

void veDirector::stopThreading()
{
	if (_sceneManager.valid())
		_sceneManager->stopThreading();
}

bool veDirector::frame(double deltaTime)
{
	_sceneManager->setDeltaTime(deltaTime);
	return _sceneManager->simulation();
}
