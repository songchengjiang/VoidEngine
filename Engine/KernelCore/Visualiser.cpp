#include "Visualiser.h"
#include "Node.h"

veVisualiser::veVisualiser(int w, int h, const std::string &title)
	: USE_VE_PTR_INIT
	, _width(w)
	, _height(h)
	, _title(title)
{
	_hwnd = glfwCreateWindow(_width, _height, title.c_str(), nullptr, nullptr);
	auto camera = new veCamera({ 0, 0, _width, _height });
	camera->setProjectionMatrixAsPerspective(30.0f, (float)_width / (float)_height, 1.0f, 1000.0f);
	addCamera(camera);
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

int veVisualiser::addCamera(veCamera *camera)
{
	auto iter = std::find(_cameras.begin(), _cameras.end(), camera);
	if (iter != _cameras.end()) return -1;
	_cameras.push_back(camera);
	return _cameras.size() - 1;
}

veCamera* veVisualiser::getCamera(unsigned int idx)
{
	veAssert(idx < _cameras.size());
	return _cameras[idx].get();
}

veCamera* veVisualiser::removeCamera(unsigned int idx)
{
	veAssert(idx < _cameras.size());
	veCamera* cam = _cameras[idx].get();
	_cameras.erase(_cameras.begin() + idx);
	return cam;
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
	else if (event.getEventType() == veEvent::VE_WIN_RESIZE) {
		_width = event.getWindowWidth();
		_height = event.getWindowHeight();
		glViewport(0, 0, _width, _height);
	}
	_deltaTime = deltaTime;

	if (_root.valid()){
		return _root->routeEvent(event, this);
	}
	return false;
}

void veVisualiser::update()
{
	if (_root.valid())
		_root->update(this);
}

void veVisualiser::render()
{
	veRenderQueue::CURRENT_RENDER_QUEUE = &_renderQueue;
	for (auto &iter : _cameras) {
		_root->render(iter.get());
	}
	_renderQueue.execute();
	glfwSwapBuffers(_hwnd);
}
