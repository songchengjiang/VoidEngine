#include "SceneManager.h"
#include "Event.h"
#include "EventDispatcher.h"
#include "Surface.h"
#include "Image.h"
#include "Text.h"
#include "Entity.h"
#include "SkyBox.h"
#include "Animation.h"

#include "LightManager.h"
#include "TextureManager.h"
#include "EntityManager.h"
#include "AnimationManager.h"

veSceneManager::veSceneManager()
	: USE_VE_PTR_INIT
	, _deltaTime(0.0)
	, _stopThreading(true)
	, _isInited(false)
{
	_managerList[veLightManager::TYPE()] = new veLightManager(this);
	_managerList[veTextureManager::TYPE()] = new veTextureManager(this);
	_managerList[veEntityManager::TYPE()] = new veEntityManager(this);
	_managerList[veAnimationManager::TYPE()] = new veAnimationManager(this);
}

veSceneManager::~veSceneManager()
{
	stopThreading();
	for (auto &iter : _managerList) {
		VE_SAFE_DELETE(iter.second);
	}
}

veVisualiser* veSceneManager::createVisualiser(int w, int h, const std::string &title)
{
	auto visualiser = new veVisualiser(w, h, title);
	veVisualiserRegistrar::instance()->reg(visualiser->_hwnd, visualiser);
//	glfwMakeContextCurrent(visualiser->_hwnd);
//#if defined(_MSC_VER)
//	if (glewInit() != GLEW_OK) veLog("glewInit error!");
//#endif
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

veSurface* veSceneManager::createSurface(const std::string &name)
{
	auto surface = new veSurface;
	surface->_sceneManager = this;
	surface->setName(name);
	return surface;
}

veImage* veSceneManager::createImage(const std::string &name, veTexture *texture)
{
	auto image = new veImage(texture);
	image->_sceneManager = this;
	image->setName(name);
	return image;
}

veEntity* veSceneManager::createEntity(const std::string &name)
{
	return static_cast<veEntityManager *>(_managerList[veEntityManager::TYPE()])->createEntity(name);
}

veSkyBox* veSceneManager::createSkyBox(const std::string &name, veReal size)
{
	auto skybox = new veSkyBox(size);
	skybox->_sceneManager = this;
	skybox->setName(name);
	return skybox;
}

veText* veSceneManager::createText(const std::string &name, veFont *font, const std::string &content)
{
	auto text = new veText(font, content);
	text->setName(name);
	text->_sceneManager = this;
	return text;
}

veAnimationContainer* veSceneManager::createAnimationContainer(const std::string &name)
{
	return static_cast<veAnimationManager *>(_managerList[veAnimationManager::TYPE()])->createAnimationContainer(name);
}

veAnimation* veSceneManager::createAnimation(const std::string &name)
{
	return static_cast<veAnimationManager *>(_managerList[veAnimationManager::TYPE()])->createAnimation(name);
}

veAnimationPlayer* veSceneManager::createAnimationPlayer(const std::string &name, veAnimationContainer *container)
{
	return static_cast<veAnimationManager *>(_managerList[veAnimationManager::TYPE()])->createAnimationPlayer(name, container);
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
	veEventDispatcher::instance()->dispatch(this);

	{
		for (auto &manager : _managerList) {
			manager.second->update();
		}
		update();
		std::unique_lock<std::mutex> lock(_renderingMutex);
		_renderingCondition.notify_all();
	}
	//render();

	return true;
}

void veSceneManager::startThreading()
{
	if (!_stopThreading) return;
	_stopThreading = false;
	_threadPool.start();
	_renderingThread = std::thread([this] {
		for (;;)
		{
			std::unique_lock<std::mutex> lock(this->_renderingMutex);
			this->_renderingCondition.wait(lock);
			if (this->_stopThreading) return;
			this->render();
		}
	});
}

void veSceneManager::stopThreading()
{
	if (_stopThreading) return;
	_renderingCondition.notify_all();
	_stopThreading = true;
	_renderingThread.join();
	_threadPool.stop();
}

void veSceneManager::render()
{
	glfwSwapBuffers(_visualiser->_hwnd);
}

void veSceneManager::makeContextCurrent()
{
	glfwMakeContextCurrent(_visualiser->_hwnd);
	if (!_isInited) {
#if defined(_MSC_VER)
		if (glewInit() != GLEW_OK) veLog("glewInit error!");
#endif
		_isInited = true;
	}
}
