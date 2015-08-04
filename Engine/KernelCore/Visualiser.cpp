#include "Visualiser.h"
#include "Node.h"

veVisualiser::veVisualiser(int w, int h, const std::string &title)
	: _width(w)
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

void veVisualiser::setSceneNode(veNode *node)
{
	_root = node;
}

bool veVisualiser::simulate(double deltaTime)
{
	if (glfwWindowShouldClose(_hwnd)) return false;

	update(deltaTime);
	render(deltaTime);

	return true;
}

bool veVisualiser::dispatchEvent(const veEvent &event)
{
	if (_root.valid()){
		return _root->routeEvent(event, this);
	}
	return false;
}

void veVisualiser::update(double deltaTime)
{
	if (_root.valid()){
		_root->update(deltaTime, this);
	}
}

void veVisualiser::render(double deltaTime)
{
	glfwMakeContextCurrent(_hwnd);
	glClear(_clearMask);
	glClearColor(_clearColor.r(), _clearColor.g(), _clearColor.b(), _clearColor.a());
	glfwSwapBuffers(_hwnd);
}
