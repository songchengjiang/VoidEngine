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

void veEventDispatcher::initEventCallbacks()
{
	veRegistrar<GLFWwindow*, veVisualiser>::instance()->addRegisterCallback([](GLFWwindow *wnd, veVisualiser *v ){
		glfwSetKeyCallback(wnd, collectKeyEvent);
		glfwSetCharCallback(wnd, collectCharEvent);
		glfwSetMouseButtonCallback(wnd, collectMouseEvent);
		glfwSetCursorPosCallback(wnd, collectMouseMoveEvent);
		glfwSetScrollCallback(wnd, collectScrollEvent);
		glfwSetWindowSizeCallback(wnd, collectWindowSizeEvent);
		glfwSetWindowFocusCallback(wnd, collectWindowFocusEvent);
	});

	veRegistrar<GLFWwindow*, veVisualiser>::instance()->addUnRegisterCallback([](GLFWwindow *wnd, veVisualiser *v){
		glfwSetKeyCallback(wnd, nullptr);
		glfwSetCharCallback(wnd, nullptr);
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
	_mouseUnitCoords = veVec2(coordX - 0.5, coordY - 0.5);
}

void veEventDispatcher::collectKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

void veEventDispatcher::collectCharEvent(GLFWwindow* window, unsigned int codepoint)
{

}

void veEventDispatcher::collectMouseEvent(GLFWwindow* window, int button, int action, int mods)
{

}

void veEventDispatcher::collectMouseMoveEvent(GLFWwindow* window, double x, double y)
{

}

void veEventDispatcher::collectScrollEvent(GLFWwindow* window, double x, double y)
{

}

void veEventDispatcher::collectWindowSizeEvent(GLFWwindow* window, int width, int height)
{

}

void veEventDispatcher::collectWindowFocusEvent(GLFWwindow* window, int focused)
{

}

veEventDispatcher* veEventDispatcher::instance()
{
	static veEventDispatcher dispatcher;
	return &dispatcher;
}
