#include "EventDispatcher.h"
#include "Visualiser.h"
#include "Registrar.h"

veEventDispatcher::~veEventDispatcher()
{

}

veEventDispatcher::veEventDispatcher()
{
	initEventCallbacks();
}

veEventDispatcher* veEventDispatcher::instance()
{
	static veEventDispatcher dispatcher;
	return &dispatcher;
}

void veEventDispatcher::dispatch()
{

}

void veEventDispatcher::initEventCallbacks()
{
	veRegistrar<GLFWwindow*, veVisualiser>::instance()->addRegisterCallback([](GLFWwindow *wnd, veVisualiser *vs){
		glfwSetKeyCallback(wnd, collectKeyEvent);
		//glfwSetCharModsCallback(wnd, collectCharEvent);
		glfwSetMouseButtonCallback(wnd, collectMouseEvent);
		glfwSetCursorPosCallback(wnd, collectMouseMoveEvent);
		glfwSetScrollCallback(wnd, collectScrollEvent);
		glfwSetWindowSizeCallback(wnd, collectWindowSizeEvent);
		glfwSetWindowFocusCallback(wnd, collectWindowFocusEvent);
	});

	veRegistrar<GLFWwindow*, veVisualiser>::instance()->addUnRegisterCallback([](GLFWwindow *wnd, veVisualiser *vs){
		glfwSetKeyCallback(wnd, nullptr);
		//glfwSetCharModsCallback(wnd, nullptr);
		glfwSetMouseButtonCallback(wnd, nullptr);
		glfwSetCursorPosCallback(wnd, nullptr);
		glfwSetScrollCallback(wnd, nullptr);
		glfwSetWindowSizeCallback(wnd, nullptr);
		glfwSetWindowFocusCallback(wnd, nullptr);
	});
}

void veEventDispatcher::caculateMouseUnitCoords(veVisualiser *vs, double x, double y)
{
	double coordX = x / (double)vs->width();
	double coordY = 1.0 - y / (double)vs->height();
	veEventDispatcher::instance()->_currentEvent.setMouseX(coordX - 0.5);
	veEventDispatcher::instance()->_currentEvent.setMouseX(coordY - 0.5);
}

void veEventDispatcher::collectKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	veEvent &event = veEventDispatcher::instance()->_currentEvent;
	event.setKeySymbol(key);
	event.setEventType(action == GLFW_PRESS? veEvent::VE_DOWN: veEvent::VE_UP);
	event.setModKeySymbol((veEvent::ModKeySymbol)mods);
	veEventDispatcher::instance()->_events[window].push_back(event);
}

//void veEventDispatcher::collectCharEvent(GLFWwindow* window, unsigned int codepoint, int mods)
//{
//	veEvent event;
//	event.setMouseX(veEventDispatcher::instance()->_mouseUnitCoords.x());
//	event.setMouseY(veEventDispatcher::instance()->_mouseUnitCoords.y());
//	event.setKeySymbol(codepoint);
//	event.setModKeySymbol((veEvent::ModKeySymbol)mods);
//	veEventDispatcher::instance()->_events[window].push_back(event);
//}

void veEventDispatcher::collectMouseEvent(GLFWwindow* window, int button, int action, int mods)
{
	veEvent &event = veEventDispatcher::instance()->_currentEvent;
	event.setMouseSymbol((veEvent::MouseSymbol)button);
	event.setEventType(action == GLFW_PRESS ? veEvent::VE_PRESS : veEvent::VE_RELEASE);
	event.setModKeySymbol((veEvent::ModKeySymbol)mods);
	veEventDispatcher::instance()->_events[window].push_back(event);
}

void veEventDispatcher::collectMouseMoveEvent(GLFWwindow* window, double x, double y)
{
	caculateMouseUnitCoords(veRegistrar<GLFWwindow*, veVisualiser>::instance()->getRegContent(window), x, y);
	veEvent &event = veEventDispatcher::instance()->_currentEvent;
	event.setEventType(veEvent::VE_MOVE);
	veEventDispatcher::instance()->_events[window].push_back(event);
}

void veEventDispatcher::collectScrollEvent(GLFWwindow* window, double x, double y)
{
	veEvent &event = veEventDispatcher::instance()->_currentEvent;
	event.setEventType(0.0 < y ? veEvent::VE_SCROLL_UP : veEvent::VE_SCROLL_DOWN);
	veEventDispatcher::instance()->_events[window].push_back(event);
}

void veEventDispatcher::collectWindowSizeEvent(GLFWwindow* window, int width, int height)
{
	veEvent &event = veEventDispatcher::instance()->_currentEvent;
	event.setEventType(veEvent::VE_WIN_RESIZE);
	event.setWindowWidth(width);
	event.setWindowHeight(height);
	veEventDispatcher::instance()->_events[window].push_back(event);
}

void veEventDispatcher::collectWindowFocusEvent(GLFWwindow* window, int focused)
{
	if (focused){
		veEvent &event = veEventDispatcher::instance()->_currentEvent;
		event.setEventType(veEvent::VE_WIN_FOCUS);
		veEventDispatcher::instance()->_events[window].push_back(event);
	}
}

