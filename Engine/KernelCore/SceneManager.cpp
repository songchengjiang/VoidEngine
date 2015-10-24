#include "SceneManager.h"
#include "Event.h"
#include "Overlay.h"
#include "Entity.h"
#include "Text.h"
#include "Animation.h"

#include "LightManager.h"
#include "TextureManager.h"
#include "EntityManager.h"

veSceneManager::veSceneManager()
	: _deltaTime(0.0)
	, _renderQueue(nullptr)
{
	_managerList[veLightManager::TYPE()] = new veLightManager(this);
	_managerList[veTextureManager::TYPE()] = new veTextureManager(this);
	_managerList[veEntityManager::TYPE()] = new veEntityManager(this);
}

veSceneManager::~veSceneManager()
{
	for (auto &iter : _managerList) {
		VE_SAFE_DELETE(iter.second);
	}
	VE_SAFE_DELETE(_renderQueue);
}

veVisualiser* veSceneManager::createVisualiser(int w, int h, const std::string &title)
{
	auto visualiser = new veVisualiser(w, h, title);
	veVisualiserRegistrar::instance()->reg(visualiser->_hwnd, visualiser);
	glfwMakeContextCurrent(visualiser->_hwnd);
#if defined(_MSC_VER)
	if (glewInit() != GLEW_OK) veLog("glewInit error!");
#endif
	visualiser->_sceneManager = this;
	if (_visualiser.valid()) veVisualiserRegistrar::instance()->unReg(_visualiser->_hwnd);
	_visualiser = visualiser;
	return visualiser;
}

veLight* veSceneManager::createLight(const std::string &type, const std::string &name)
{
	auto light = static_cast<veLightManager *>(_managerList[veLightManager::TYPE()])->createLight(type, name);
	_lightList.push_back(light);
	return light;
}

veOverlay* veSceneManager::createOverlay(const std::string &name)
{
	auto overlay = new veOverlay;
	overlay->setName(name);
	return overlay;
}

veEntity* veSceneManager::createEntity(const std::string &name)
{
	return static_cast<veEntityManager *>(_managerList[veEntityManager::TYPE()])->createEntity(name);
}

veText* veSceneManager::createText(veFont *font, const std::string &content)
{
	auto text = new veText(font, content);
	return text;
}

veAnimationContainer* veSceneManager::createAnimationContainer(const std::string &name)
{
	auto anim = new veAnimationContainer;
	anim->setName(name);
	return anim;
}

veAnimation* veSceneManager::createAnimation(const std::string &name)
{
	auto anim = new veAnimation;
	anim->setName(name);
	return anim;
}

veTexture* veSceneManager::createTexture(const std::string &name, veTexture::TextureType texType)
{
	return static_cast<veTextureManager *>(_managerList[veTextureManager::TYPE()])->createTexture(name, texType);
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
	render();

	return true;
}

void veSceneManager::render()
{
	glfwSwapBuffers(_visualiser->_hwnd);
}
