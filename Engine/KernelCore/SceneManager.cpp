#include "SceneManager.h"
#include "Event.h"

veSceneManager::veSceneManager()
	: _deltaTime(0.0)
{

}

veSceneManager::~veSceneManager()
{

}

veVisualiser* veSceneManager::createVisualiser(int w, int h, const std::string &title)
{
	auto visualiser = new veVisualiser(w, h, title);
	veVisualiserRegistrar::instance()->reg(visualiser->_hwnd, visualiser);
	glfwMakeContextCurrent(visualiser->_hwnd);
	if (glewInit() != GLEW_OK) {
		veLog("glewInit error!")
	}
	visualiser->_sceneManager = this;
	if (_visualiser.valid()) veVisualiserRegistrar::instance()->unReg(_visualiser->_hwnd);
	_visualiser = visualiser;
	return visualiser;
}

void veSceneManager::dispatchEvents(double deltaTime, veEvent &event)
{
	_deltaTime = deltaTime;
	if (event.getEventType() == veEvent::VE_WIN_RESIZE) {
		_visualiser->_width = event.getWindowWidth();
		_visualiser->_height = event.getWindowHeight();
	}
	if (_root.valid())
		_root->routeEvent(event, _visualiser.get());
}

bool veSceneManager::simulation(double deltaTime)
{
	if (glfwWindowShouldClose(_visualiser->_hwnd)) return false;
	glfwMakeContextCurrent(_visualiser->_hwnd);

	update();
	cull();
	render();

	return true;
}

void veSceneManager::render()
{
	glfwSwapBuffers(_visualiser->_hwnd);
}
