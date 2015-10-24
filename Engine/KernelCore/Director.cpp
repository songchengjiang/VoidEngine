#include "Director.h"
#include "EventDispatcher.h"
#include "SceneManager.h"
#include "OctreeSceneManager.h"

veDirector::veDirector()
	: _sceneManager(nullptr)
	, _isRunning(false)
{
	glfwInit();
#if defined(__APPLE_CC__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, VE_GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, VE_GL_VERSION_MINOR);
#endif
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

//veVisualiser* veDirector::createVisualiser(int w, int h, const std::string &title)
//{
//	auto visualiser = new veVisualiser(w, h, title);
//	veVisualiserRegistrar::instance()->reg(visualiser->_hwnd, visualiser);
//	_visualiserList.push_back(visualiser);
//	return visualiser;
//}

bool veDirector::run()
{
    _isRunning = true;
	double preFrameTime = glfwGetTime();
	while (_isRunning)
	{
		double currentFrameTime = glfwGetTime();
		_sceneManager->setDeltaTime(currentFrameTime - preFrameTime);
		veEventDispatcher::instance()->dispatch(_sceneManager);
		if (!_sceneManager->simulation()) _isRunning = false;
		preFrameTime = currentFrameTime;
	}
	return true;
}

void veDirector::stop()
{
	_isRunning = false;
}
