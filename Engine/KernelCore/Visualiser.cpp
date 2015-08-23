#include "Visualiser.h"
#include "Node.h"

veVisualiser::veVisualiser(int w, int h, const std::string &title)
	: USE_VE_PTR_INIT
	, _width(w)
	, _height(h)
	, _title(title)
	, _clearColor(veVec4::WHITE)
	, _clearMask(GL_COLOR_BUFFER_BIT)
{
	_hwnd = glfwCreateWindow(_width, _height, title.c_str(), nullptr, nullptr);
}

veVisualiser::~veVisualiser()
{
	glfwDestroyWindow(_hwnd);
}

int veVisualiser::width()
{
	glfwGetWindowSize(_hwnd, &_width, &_height);
	return _width;
}

int veVisualiser::height()
{
	glfwGetWindowSize(_hwnd, &_width, &_height);
	return _height;
}

void veVisualiser::setSceneNode(veNode *node)
{
	_root = node;
}

bool veVisualiser::simulate(double deltaTime)
{
	if (glfwWindowShouldClose(_hwnd)) 
		return false;
	_deltaTime = deltaTime;
	update();
	render();
	return true;
}

bool veVisualiser::dispatchEvent(double deltaTime, const veEvent &event)
{
	if (event.getEventType() == veEvent::VE_WIN_CLOSE) return true;
	_deltaTime = deltaTime;
	if (_root.valid()){
		return _root->routeEvent(event, this);
	}
	return false;
}

void veVisualiser::update()
{
	if (_root.valid()){
		_root->update(this);
	}
}

void veVisualiser::render()
{
	glClear(_clearMask);
	glClearColor(_clearColor.r(), _clearColor.g(), _clearColor.b(), _clearColor.a());
	_renderQueue.execute(this);
	glfwSwapBuffers(_hwnd);
}
