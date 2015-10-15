#include "SceneManager.h"
#include "Event.h"
#include "LightManager.h"
#include "Overlay.h"
#include "Mesh.h"
#include "Text.h"
#include "Animation.h"

veSceneManager::veSceneManager()
	: _deltaTime(0.0)
{
	_managerList[veLightManager::TYPE()] = new veLightManager;
}

veSceneManager::~veSceneManager()
{
	for (auto &iter : _managerList) {
		VE_SAFE_DELETE(iter.second);
	}
}

veVisualiser* veSceneManager::createVisualiser(int w, int h, const std::string &title)
{
	auto visualiser = new veVisualiser(w, h, title);
	veVisualiserRegistrar::instance()->reg(visualiser->_hwnd, visualiser);
	glfwMakeContextCurrent(visualiser->_hwnd);
	if (glewInit() != GLEW_OK) {
		veLog("glewInit error!")
	}
	visualiser->_sceneManager = this;
	if (_visualiser.valid()) veVisualiserRegistrar::instance()->unReg(_visualiser->_hwnd);
	_visualiser = visualiser;
	return visualiser;
}

veLight* veSceneManager::createLight(const std::string &type)
{
	auto light = static_cast<veLightManager *>(_managerList[veLightManager::TYPE()])->createLight(type);
	_lightList.push_back(light);
	return light;
}

veOverlay* veSceneManager::createOverlay()
{
	auto overlay = new veOverlay;
	return overlay;
}

veMesh* veSceneManager::createMesh()
{
	auto mesh = new veMesh;
	return mesh;
}

veText* veSceneManager::createText(veFont *font, const std::string &content)
{
	auto text = new veText(font, content);
	return text;
}

veAnimation* veSceneManager::createAnimation()
{
	auto anim = new veAnimation;
	return anim;
}

void veSceneManager::loadLightConfiguration(const std::string &filePath)
{
	static_cast<veLightManager *>(_managerList[veLightManager::TYPE()])->loadLightTemplates(filePath);
}

veBaseManager* veSceneManager::getManager(const std::string &mgType)
{
	auto iter = _managerList.find(mgType);
	if (iter == _managerList.end()) return nullptr;

	return iter->second;
}

void veSceneManager::dispatchEvents(veEvent &event)
{
	if (event.getEventType() == veEvent::VE_WIN_RESIZE) {
		_visualiser->_width = event.getWindowWidth();
		_visualiser->_height = event.getWindowHeight();
	}
	if (_root.valid())
		_root->routeEvent(event, this);
}

bool veSceneManager::simulation()
{
	if (glfwWindowShouldClose(_visualiser->_hwnd)) return false;
	glfwMakeContextCurrent(_visualiser->_hwnd);

	update();
	cull();
	render();

	return true;
}

void veSceneManager::render()
{
	glfwSwapBuffers(_visualiser->_hwnd);
}
