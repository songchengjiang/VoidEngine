#include "Director.h"
#include "EventDispatcher.h"

veDirector::veDirector()
	: _isRunning(false)
{
	glfwInit();
	veEventDispatcher::instance()->registerCallback();
}

veDirector::~veDirector()
{
	glfwTerminate();
}

veDirector* veDirector::instance()
{
	static veDirector director;
	return &director;
}

veVisualiser* veDirector::createVisualiser(int w, int h, const std::string &title)
{
	auto visualiser = new veVisualiser(w, h, title);
	veVisualiserRegistrar::instance()->reg(visualiser->_hwnd, visualiser);
	_visualiserList.push_back(visualiser);
	return visualiser;
}

bool veDirector::run()
{
	_isRunning = true;
	double preFrameTime = glfwGetTime();
	while (_isRunning)
	{
		double currentFrameTime = glfwGetTime();
		double deltaTime = currentFrameTime - preFrameTime;
		veEventDispatcher::instance()->dispatch(deltaTime);
		for (auto &iter : _visualiserList){
			if (!iter->simulate(deltaTime)) stop();
		}
		preFrameTime = currentFrameTime;
	}
	return true;
}

void veDirector::stop()
{
	_isRunning = false;
}
