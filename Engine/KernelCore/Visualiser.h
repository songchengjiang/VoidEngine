#ifndef _VE_VISUALISER_
#define _VE_VISUALISER_
#include <string>
#include "Prerequisites.h"
#include "Registrar.h"

class VE_EXPORT veVisualiser
{
public:
	veVisualiser(int w, int h, const std::string &title);
	~veVisualiser();

	int width() const { return _width; }
	int height() const { return _height; }

private:

	int _width;
	int _height;
	std::string _title;
	GLFWwindow *_hwnd;
};

typedef veRegistrar<GLFWwindow*, veVisualiser> veVisualiserRegistrar;

#endif