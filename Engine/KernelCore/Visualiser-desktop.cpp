#include "Visualiser-desktop.h"
#include "SceneManager.h"

veVisualiserPC::veVisualiserPC(int w, int h, const std::string &title)
	: veVisualiser(w, h, title)
	, _width(w)
	, _height(h)
	, _title(title)
	, _isInited(false)
	, _sceneManager(nullptr)
{
	_hwnd = glfwCreateWindow(_width, _height, title.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(_hwnd, this);
	registerCallbacks();
	//veVisualiserRegistrar::instance()->reg(_hwnd, this);
	//_mainCamera = new veCamera({ 0, 0, _width, _height });
	//_mainCamera->setProjectionMatrixAsPerspective(30.0f, (float)_width / (float)_height, 1.0f, 1000.0f);
}

veVisualiserPC::~veVisualiserPC()
{
	//veVisualiserRegistrar::instance()->unReg(_hwnd);
	unRegisterCallbacks();
	glfwDestroyWindow(_hwnd);
}

bool veVisualiserPC::dispatchEvents()
{
	glfwPollEvents();
	for (auto &event : _events) {
		//auto vs = veVisualiserRegistrar::instance()->getRegContent(event.first);
		_sceneManager->dispatchEvents(event);
	}
	_events.clear();
	return false;
}

void veVisualiserPC::setContextCurrent()
{
	glfwMakeContextCurrent(_hwnd);
	if (!_isInited) {
#if defined(_MSC_VER)
		if (glewInit() != GLEW_OK) veLog("glewInit error!");
#endif
		_isInited = true;
	}
}

void veVisualiserPC::swapBuffers()
{
	glfwSwapBuffers(_hwnd);
}

bool veVisualiserPC::isWindowShouldClose()
{
	return glfwWindowShouldClose(_hwnd) != 0;
}

void veVisualiserPC::registerCallbacks()
{
	glfwSetKeyCallback(_hwnd, collectKeyEvent);
	//glfwSetCharModsCallback(wnd, collectCharEvent);
	glfwSetMouseButtonCallback(_hwnd, collectMouseEvent);
	glfwSetCursorPosCallback(_hwnd, collectMouseMoveEvent);
	glfwSetScrollCallback(_hwnd, collectScrollEvent);
	glfwSetWindowSizeCallback(_hwnd, collectWindowSizeEvent);
	glfwSetWindowFocusCallback(_hwnd, collectWindowFocusEvent);
	glfwSetWindowCloseCallback(_hwnd, collectWindowClose);
}

void veVisualiserPC::unRegisterCallbacks()
{
	glfwSetKeyCallback(_hwnd, nullptr);
	//glfwSetCharModsCallback(wnd, nullptr);
	glfwSetMouseButtonCallback(_hwnd, nullptr);
	glfwSetCursorPosCallback(_hwnd, nullptr);
	glfwSetScrollCallback(_hwnd, nullptr);
	glfwSetWindowSizeCallback(_hwnd, nullptr);
	glfwSetWindowFocusCallback(_hwnd, nullptr);
	glfwSetWindowCloseCallback(_hwnd, nullptr);
}

void veVisualiserPC::caculateMouseUnitCoords(GLFWwindow* window, double x, double y)
{
	veVisualiserPC *vs = static_cast<veVisualiserPC *>(glfwGetWindowUserPointer(window));
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if (width == 0 || height == 0) return;
	double coordX = x / (double)width;
	double coordY = 1.0 - y / (double)height;
	vs->_currentEvent.setMouseX((coordX - 0.5) * 2.0);
	vs->_currentEvent.setMouseY((coordY - 0.5) * 2.0);
}

void veVisualiserPC::collectKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	veVisualiserPC *vs = static_cast<veVisualiserPC *>(glfwGetWindowUserPointer(window));
	veEvent &event = vs->_currentEvent;
	event.setKeySymbol(key);
	event.setEventType(action == GLFW_PRESS ? veEvent::VE_DOWN : veEvent::VE_UP);
	event.setModKeySymbol((veEvent::ModKeySymbol)mods);
	vs->_events.push_back(event);
}

void veVisualiserPC::collectMouseEvent(GLFWwindow* window, int button, int action, int mods)
{
	veVisualiserPC *vs = static_cast<veVisualiserPC *>(glfwGetWindowUserPointer(window));
	veEvent &event = vs->_currentEvent;
	event.setMouseSymbol((veEvent::MouseSymbol)button);
	event.setEventType(action == GLFW_PRESS ? veEvent::VE_PRESS : veEvent::VE_RELEASE);
	event.setModKeySymbol((veEvent::ModKeySymbol)mods);
	vs->_events.push_back(event);
}

void veVisualiserPC::collectMouseMoveEvent(GLFWwindow* window, double x, double y)
{
	veVisualiserPC *vs = static_cast<veVisualiserPC *>(glfwGetWindowUserPointer(window));
	caculateMouseUnitCoords(window, x, y);
	veEvent &event = vs->_currentEvent;
	event.setEventType(veEvent::VE_MOVE);
	auto action = glfwGetMouseButton(window, event.getMouseSymbol());
	if (action != GLFW_PRESS) event.setMouseSymbol(veEvent::VE_MOUSE_UNKNOWN);
	vs->_events.push_back(event);
}

void veVisualiserPC::collectScrollEvent(GLFWwindow* window, double x, double y)
{
	veVisualiserPC *vs = static_cast<veVisualiserPC *>(glfwGetWindowUserPointer(window));
	veEvent &event = vs->_currentEvent;
	event.setEventType(0.0 < y ? veEvent::VE_SCROLL_UP : veEvent::VE_SCROLL_DOWN);
	vs->_events.push_back(event);
}

void veVisualiserPC::collectWindowSizeEvent(GLFWwindow* window, int width, int height)
{
	if (width == 0 || height == 0) return;
	veVisualiserPC *vs = static_cast<veVisualiserPC *>(glfwGetWindowUserPointer(window));
	veEvent &event = vs->_currentEvent;
	event.setEventType(veEvent::VE_WIN_RESIZE);
	event.setWindowWidth(width);
	event.setWindowHeight(height);
	vs->_events.push_back(event);
}

void veVisualiserPC::collectWindowFocusEvent(GLFWwindow* window, int focused)
{
	if (focused) {
		veVisualiserPC *vs = static_cast<veVisualiserPC *>(glfwGetWindowUserPointer(window));
		veEvent &event = vs->_currentEvent;
		event.setEventType(veEvent::VE_WIN_FOCUS);
		vs->_events.push_back(event);
	}
}

void veVisualiserPC::collectWindowClose(GLFWwindow* window)
{
	veVisualiserPC *vs = static_cast<veVisualiserPC *>(glfwGetWindowUserPointer(window));
	veEvent &event = vs->_currentEvent;
	event.setEventType(veEvent::VE_WIN_CLOSE);
	vs->_events.push_back(event);
}
