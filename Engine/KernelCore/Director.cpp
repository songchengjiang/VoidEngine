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
	double preFrameTime = glfwGetTime();
	if (!_visualiserList.empty()) {
		glfwMakeContextCurrent(_visualiserList[0]->_hwnd);
		_isRunning = glewInit() == GLEW_OK;
	}
	while (_isRunning)
	{
		double currentFrameTime = glfwGetTime();
		double deltaTime = currentFrameTime - preFrameTime;
		//char str[64];
		//sprintf(str, "Frame: %.1f", 1.0 / deltaTime);
		//veLog(str);
		veEventDispatcher::instance()->dispatch(deltaTime);
		for (auto &iter : _visualiserList){
			glfwMakeContextCurrent(iter->_hwnd);
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
