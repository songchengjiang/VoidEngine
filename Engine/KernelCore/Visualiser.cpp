#include "Visualiser.h"

veVisualiser::veVisualiser(int w, int h, const std::string &title)
	: _width(w)
	, _height(h)
	, _title(title)
{
	_hwnd = glfwCreateWindow(_width, _height, title.c_str(), nullptr, nullptr);
}

veVisualiser::~veVisualiser()
{

}
