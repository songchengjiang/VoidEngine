#include "Visualiser.h"
#include "Node.h"
#include "NodeVisitor.h"

class NodeFinder : public veNodeVisitor
{
public:
	NodeFinder(veCameraList &cameraList, veCamera *discardCam, veLightList &lightList)
		: _cameraList(cameraList)
	    , _discardCam(discardCam)
		, _lightList(lightList){

	}
	~NodeFinder() {

	}

	virtual void visit(veCamera &camera) override{
		if (&camera != _discardCam && camera.isVisible())
			_cameraList.push_back(&camera);
	}

	virtual void visit(veLight &light) override {
		if (light.isVisible())
			_lightList.push_back(&light);
	}

private:

	veCameraList &_cameraList;
	veCamera   *_discardCam;
	veLightList &_lightList;
};

veVisualiser::veVisualiser(int w, int h, const std::string &title)
	: USE_VE_PTR_INIT
	, _width(w)
	, _height(h)
	, _title(title)
{
	_hwnd = glfwCreateWindow(_width, _height, title.c_str(), nullptr, nullptr);
	_mainCamera = new veCamera({ 0, 0, _width, _height });
	_mainCamera->setProjectionMatrixAsPerspective(30.0f, (float)_width / (float)_height, 1.0f, 1000.0f);
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
		resize(event.getWindowWidth(), event.getWindowHeight());
	}
	_deltaTime = deltaTime;

	bool state = false;
	if (_root.valid()){
		state = _root->routeEvent(event, this);
	}
	if (!state && !_mainCamera->getParent() && _root.get() != _mainCamera.get())
		_mainCamera->routeEvent(event, this);
	return state;
}

void veVisualiser::update()
{
	if (!_root.valid()) return;
	_root->update(this);
	_cameras.clear();
	_lights.clear();
	NodeFinder nodeFinder(_cameras, _mainCamera.get(), _lights);
	_root->accept(nodeFinder);
	if (!_mainCamera->getParent() && _root.get() != _mainCamera.get())
		_mainCamera->update(this);
}

void veVisualiser::render()
{
	if (!_root.valid()) return;
	//glClear(_clearMask);
	veRenderQueue::CURRENT_RENDER_QUEUE = &_renderQueue;
	for (auto &iter : _cameras) {
		if (iter->getFrameBufferObject())
			_root->render(iter);
	}
	_root->render(_mainCamera.get());
	_renderQueue.execute(this);
	glfwSwapBuffers(_hwnd);
}

void veVisualiser::resize(int width, int height)
{
	double widthChangeRatio = double(width) / double(_width);
	double heigtChangeRatio = double(height) / double(_height);
	double aspectInverseRatioChange = heigtChangeRatio / widthChangeRatio;
	_width = width;
	_height = height;

	_mainCamera->projectionMatrix() = veMat4::scale(veVec3(aspectInverseRatioChange, 1.0f, 1.0f)) * _mainCamera->projectionMatrix();
	_mainCamera->setViewport({ 0, 0, _width, _height });
}