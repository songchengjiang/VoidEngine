#include "EventDispatcher.h"
#include "Visualiser.h"
#include "Node.h"

veEventDispatcher::~veEventDispatcher()
{

}

veEventDispatcher::veEventDispatcher()
{
}

veEventDispatcher* veEventDispatcher::instance()
{
	static veEventDispatcher dispatcher;
	return &dispatcher;
}

void veEventDispatcher::registerCallback()
{
	veVisualiserRegistrar::instance()->addRegisterCallback([](GLFWwindow *wnd, veVisualiser *vs){
		glfwSetKeyCallback(wnd, collectKeyEvent);
		//glfwSetCharModsCallback(wnd, collectCharEvent);
		glfwSetMouseButtonCallback(wnd, collectMouseEvent);
		glfwSetCursorPosCallback(wnd, collectMouseMoveEvent);
		glfwSetScrollCallback(wnd, collectScrollEvent);
		glfwSetWindowSizeCallback(wnd, collectWindowSizeEvent);
		glfwSetWindowFocusCallback(wnd, collectWindowFocusEvent);
		glfwSetWindowCloseCallback(wnd, collectWindowClose);
	});

	veVisualiserRegistrar::instance()->addUnRegisterCallback([](GLFWwindow *wnd, veVisualiser *vs){
		glfwSetKeyCallback(wnd, nullptr);
		//glfwSetCharModsCallback(wnd, nullptr);
		glfwSetMouseButtonCallback(wnd, nullptr);
		glfwSetCursorPosCallback(wnd, nullptr);
		glfwSetScrollCallback(wnd, nullptr);
		glfwSetWindowSizeCallback(wnd, nullptr);
		glfwSetWindowFocusCallback(wnd, nullptr);
		glfwSetWindowCloseCallback(wnd, nullptr);
	});
}

bool veEventDispatcher::dispatch(double deltaTime)
{
	glfwPollEvents();
	for (auto &event : _events) {
		auto vs = veVisualiserRegistrar::instance()->getRegContent(event.first);
		const auto &events = event.second;
		for (auto e : events) {
			vs->dispatchEvent(deltaTime, e);
		}
	}

	_events.clear();
	return false;
}

void veEventDispatcher::caculateMouseUnitCoords(GLFWwindow* window, double x, double y)
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	double coordX = x / (double)width;
	double coordY = 1.0 - y / (double)height;
	veEventDispatcher::instance()->_currentEvent.setMouseX(coordX - 0.5);
	veEventDispatcher::instance()->_currentEvent.setMouseY(coordY - 0.5);
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
	caculateMouseUnitCoords(window, x, y);
	veEvent &event = veEventDispatcher::instance()->_currentEvent;
	event.setEventType(veEvent::VE_MOVE);
	auto action = glfwGetMouseButton(window, event.getMouseSymbol());
	if (action != GLFW_PRESS) event.setMouseSymbol(veEvent::VE_MOUSE_UNKNOWN);
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

void veEventDispatcher::collectWindowClose(GLFWwindow* window)
{
	veEvent &event = veEventDispatcher::instance()->_currentEvent;
	event.setEventType(veEvent::VE_WIN_CLOSE);
	veEventDispatcher::instance()->_events[window].push_back(event);
}
