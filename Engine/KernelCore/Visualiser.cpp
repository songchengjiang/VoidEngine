#include "Visualiser.h"
#include "Node.h"
#include "NodeVisitor.h"
#include "SceneManager.h"

veVisualiser::veVisualiser(int w, int h, const std::string &title)
	: USE_VE_PTR_INIT
	, _width(w)
	, _height(h)
	, _title(title)
	, _sceneManager(nullptr)
{
	_hwnd = glfwCreateWindow(_width, _height, title.c_str(), nullptr, nullptr);
	//_mainCamera = new veCamera({ 0, 0, _width, _height });
	//_mainCamera->setProjectionMatrixAsPerspective(30.0f, (float)_width / (float)_height, 1.0f, 1000.0f);
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

//int veVisualiser::addCamera(veCamera *camera)
//{
//	auto iter = std::find(_cameras.begin(), _cameras.end(), camera);
//	if (iter != _cameras.end()) return -1;
//	_cameras.push_back(camera);
//	return _cameras.size() - 1;
//}
//
//veCamera* veVisualiser::getCamera(unsigned int idx)
//{
//	veAssert(idx < _cameras.size());
//	return _cameras[idx].get();
//}
//
//veCamera* veVisualiser::removeCamera(unsigned int idx)
//{
//	veAssert(idx < _cameras.size());
//	veCamera* cam = _cameras[idx].get();
//	_cameras.erase(_cameras.begin() + idx);
//	return cam;
//}