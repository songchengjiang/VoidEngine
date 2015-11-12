#include "SceneManager.h"
#include "Event.h"
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

veRay* veSceneManager::createRay(const veVec3 &start, const veVec3 &end)
{
	auto ray = new veRay(start, end);
	_rayList.push_back(ray);
	return ray;
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

vePostProcesser* veSceneManager::createPostProcesser(const std::string &name)
{
	auto postProcesser = new vePostProcesser(this);
	postProcesser->setName(name);
	_postProcesserList.push_back(postProcesser);
	if (!_postProcesserFBOS[0].valid() && !_postProcesserFBOS[1].valid() && !_postProcesserTexs[0].valid() && !_postProcesserTexs[1].valid()) {
		_postProcesserFBOS[0] = veFrameBufferObjectManager::instance()->getOrCreateFrameBufferObject("postProcesserFBO[0]");
		_postProcesserFBOS[1] = veFrameBufferObjectManager::instance()->getOrCreateFrameBufferObject("postProcesserFBO[1]");
		_postProcesserTexs[0] = createTexture("_postProcesserTexs[0]");
		_postProcesserTexs[1] = createTexture("_postProcesserTexs[1]");
		_postProcesserFBOS[0]->attach(GL_COLOR_ATTACHMENT0, _postProcesserTexs[0].get());
		_postProcesserFBOS[1]->attach(GL_COLOR_ATTACHMENT0, _postProcesserTexs[1].get());
	}
	return postProcesser;
}

void veSceneManager::requestRayCast(veRay *ray)
{
	ray->_callBack();
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
	if (_root.valid())
		_root->routeEvent(event, this);
}

bool veSceneManager::simulation()
{
	{
		std::unique_lock<std::mutex> updateLock(_updatingMutex);
		for (auto &manager : _managerList) {
			manager.second->update();
		}
		update();
	}

	{
		std::unique_lock<std::mutex> renderLock(_renderingMutex);
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
		while(!this->_stopThreading)
		{
			std::unique_lock<std::mutex> renderLock(this->_renderingMutex);
			this->_renderingCondition.wait(renderLock);
			this->render();
			{
				std::unique_lock<std::mutex> updateLock(_updatingMutex);
				this->handleRequests();
			}
		}
	});
}

void veSceneManager::stopThreading()
{
	if (_stopThreading) return;
	_threadPool.stop();
	_stopThreading = true;
	_renderingCondition.notify_all();
	_renderingThread.join();
}

void veSceneManager::handleRequests()
{
	std::unique_lock<std::mutex> lock(_requestQueueMutex);
	while (!_requestQueue.empty())
	{
		auto &func = _requestQueue.front();
		func();
		_requestQueue.pop_front();
	}
}

void veSceneManager::enqueueRequest(const std::function<void()> &func)
{
	std::unique_lock<std::mutex> lock(_requestQueueMutex);
	_requestQueue.push_back(func);
}