#include "SceneManager.h"
#include "Event.h"
#include "Surface.h"
#include "Image.h"
#include "Text.h"
#include "Entity.h"
#include "Sphere.h"
#include "Cone.h"
#include "SkyBox.h"
#include "Animation.h"

#include "TextureManager.h"
#include "EntityManager.h"
#include "AnimationManager.h"
#include "MaterialManager.h"

#define RESOURCE_RECOVERY_INTERVAL_TIME 10.0

veSceneManager::veSceneManager()
	: USE_VE_PTR_INIT
	, _deltaTime(0.0)
	, _simulationTime(0)
	, _ambient(veVec3(1.0f))
	, _latestResourceRecoveredTime(RESOURCE_RECOVERY_INTERVAL_TIME)
	, _stopThreading(true)
	, _needReload(false)
{
	_managerList[veTextureManager::TYPE()] = new veTextureManager(this);
	_managerList[veEntityManager::TYPE()] = new veEntityManager(this);
	_managerList[veAnimationManager::TYPE()] = new veAnimationManager(this);
	_managerList[veMaterialManager::TYPE()] = new veMaterialManager(this);
}

veSceneManager::~veSceneManager()
{
	stopThreading();
	for (auto &iter : _managerList) {
		VE_SAFE_DELETE(iter.second);
	}
}

veLight* veSceneManager::createLight(veLight::LightType type, const std::string &name)
{
	veLight *light = nullptr;
	if (type == veLight::DIRECTIONAL) {
		light = new veDirectionalLight();
	}
	else if (type == veLight::POINT){
		light = new vePointLight();
	}
	else if (type == veLight::SPOT) {
		light = new veSpotLight();
	}
	light->setName(name);
	light->setSceneManager(this);
	_lightListMap[type].push_back(light);
	this->needReload();
	return light;
}

veSurface* veSceneManager::createSurface(const std::string &name)
{
	auto surface = new veSurface(this);
	surface->_sceneManager = this;
	surface->setName(name);
	return surface;
}

veImage* veSceneManager::createImage(const std::string &name, veTexture *texture)
{
	auto image = new veImage(this, texture);
	image->_sceneManager = this;
	image->setName(name);
	return image;
}

veEntity* veSceneManager::createEntity(const std::string &name)
{
	return static_cast<veEntityManager *>(_managerList[veEntityManager::TYPE()])->createEntity(name);
}

veSphere* veSceneManager::createSphere(const std::string &name)
{
	auto sphere = new veSphere(this);
	sphere->setName(name);
	return sphere;
}

veCone* veSceneManager::createCone(const std::string &name)
{
	auto cone = new veCone(this);
	cone->setName(name);
	return cone;
}

veSkyBox* veSceneManager::createSkyBox(const std::string &name, veReal size)
{
	auto skybox = new veSkyBox(size);
	skybox->setName(name);
	skybox->_sceneManager = this;
	return skybox;
}

veText* veSceneManager::createText(const std::string &name, veFont *font, const std::string &content)
{
	auto text = new veText(this, font, content);
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

veMaterialArray* veSceneManager::createMaterialArray(const std::string &name)
{
	return static_cast<veMaterialManager *>(_managerList[veMaterialManager::TYPE()])->createMaterialArray(name);
}

vePostProcesser* veSceneManager::createPostProcesser(const std::string &name)
{
	auto postProcesser = new vePostProcesser(this);
	postProcesser->setName(name);
	_postProcesserList.push_back(postProcesser);
	return postProcesser;
}

void veSceneManager::removePostProcesser(const std::string &name)
{
	for (vePostProcesserList::iterator iter = _postProcesserList.begin(); iter != _postProcesserList.end(); ++iter) {
		if ((*iter)->getName() == name) {
			_postProcesserList.erase(iter);
			break;
		}
	}
}

void veSceneManager::addComponent(veComponent *component)
{
	auto iter = std::find(_componentList.begin(), _componentList.end(), component);
	if (iter != _componentList.end())
		return;
	_componentList.push_back(component);
}

void veSceneManager::removeComponent(veComponent *component)
{
	auto iter = std::find(_componentList.begin(), _componentList.end(), component);
	_componentList.erase(iter);
}

void veSceneManager::requestRayCast(veRay *ray)
{
	ray->_callBack();
}

veBaseManager* veSceneManager::getManager(const std::string &mgType)
{
	auto iter = _managerList.find(mgType);
	if (iter == _managerList.end()) return nullptr;

	return iter->second;
}

void veSceneManager::setDeltaTime(double deltaTime)
{
	_deltaTime = deltaTime;
	_simulationTime += deltaTime;
}

void veSceneManager::needReload()
{
	if (_needReload)
		return;
	_needReload = true;
	enqueueRequest([this] {
		this->_needReload = false;
	});
}

void veSceneManager::dispatchEvents(veEvent &event)
{
	if (event.getEventType() == veEvent::VE_WIN_RESIZE) {
		if (_mainCamera.valid()) {
			_mainCamera->resize(event.getWindowWidth(), event.getWindowHeight());
		}
	}

	if (!_componentList.empty()) {
		for (auto &com : _componentList) {
			if (event.getEventType() & com->getEventFilter()) {
				if (com->handle(this, event)) 
					return;
			}
		}
	}

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

void veSceneManager::update()
{
	if (!_componentList.empty()) {
		for (auto &com : _componentList) {
			com->update(this);
		}
	}
}

void veSceneManager::startThreading()
{
	if (!_stopThreading) return;
	_stopThreading = false;
	_threadPool.start();
	_renderingThread = std::thread([this] {
		while(!this->_stopThreading){
			std::unique_lock<std::mutex> renderLock(this->_renderingMutex);
			this->_renderingCondition.wait(renderLock);
			this->render();
			this->postRenderHandle();
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

void veSceneManager::enqueueTaskToThread(const veThreadPool::TaskCallBack& callback, void* callbackParam, const std::function<void()> &func)
{
	_threadPool.enqueue(callback, callbackParam, func);
}

void veSceneManager::postRenderHandle()
{
	std::unique_lock<std::mutex> updateLock(_updatingMutex);
	handleRequests();
	resourceRecovery();
}

void veSceneManager::resourceRecovery()
{
	if (RESOURCE_RECOVERY_INTERVAL_TIME < _latestResourceRecoveredTime) {
		for (auto &manager : _managerList) {
			manager.second->resourceRecovery();
		}
		_latestResourceRecoveredTime = 0.0f;
	}
	_latestResourceRecoveredTime += _deltaTime;
}

void veSceneManager::handleRequests()
{
	if (_requestQueue.empty())
		return;
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