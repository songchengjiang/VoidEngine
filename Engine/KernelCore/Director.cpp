#include "Director.h"
#include "OctreeSceneManager.h"

veDirector::veDirector()
	: _isRunning(false)
{
	glfwInit();
#if defined(__APPLE_CC__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, VE_GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, VE_GL_VERSION_MINOR);
#endif
}

veDirector::~veDirector()
{
	stop();
	glfwTerminate();
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
	double preFrameTime = glfwGetTime();
	while (_isRunning)
	{
		double currentFrameTime = glfwGetTime();
		_sceneManager->setDeltaTime(currentFrameTime - preFrameTime);
		if (!_sceneManager->simulation()) _isRunning = false;
		preFrameTime = currentFrameTime;
	}
	_sceneManager->stopThreading();
	return true;
}

void veDirector::stop()
{
	_isRunning = false;
}
