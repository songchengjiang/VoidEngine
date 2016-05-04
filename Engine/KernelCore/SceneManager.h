#ifndef _VE_SCENE_MANAGER_
#define _VE_SCENE_MANAGER_
#include "Prerequisites.h"
#include "VE_Ptr.h"
#include "RenderQueue.h"
#include "ThreadPool.h"
#include "LoopQueue.h"

#include "Node.h"
#include "Camera.h"
#include "Light.h"
#include "Ray.h"
#include "SkyBox.h"
#include "PostProcesser.h"
#include "RenderPipeline.h"
#include "Component.h"
#include "EffectCore/ParticleSystem.h"

#include <unordered_map>

class veEvent;
class veNode;
class veCamera;
class veLight;
class veSurface;
class veEntity;
class veSphere;
class veCone;
class veFont;
class veText;
class veImage;
class veAnimation;
class veAnimationContainer;
class veAnimationPlayer;
class veBaseManager;
class veMaterialArray;
class veRay;

class VE_EXPORT veSceneManager
{
public:
	veSceneManager();
	virtual ~veSceneManager();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	virtual veNode* createNode(const std::string &name) = 0;
	virtual veLight* createLight(veLight::LightType type, const std::string &name);
	virtual veSurface* createSurface(const std::string &name);
	virtual veImage* createImage(const std::string &name, veTexture *texture);
	virtual veText* createText(const std::string &name, veFont *font, const std::string &content = "");
	virtual veAnimation* createAnimation(const std::string &name);
	virtual veAnimationContainer* createAnimationContainer(const std::string &name);

	virtual veRay* createRay(const veVec3 &start, const veVec3 &end);
	virtual veCamera* createCamera(const std::string &name, const veViewport &vp = { 0, 0, 0, 0 }) = 0;
	virtual veEntity* createEntity(const std::string &name);
	virtual veSphere* createSphere(const std::string &name);
	virtual veCone* createCone(const std::string &name);
	virtual veSkyBox* createSkyBox(const std::string &name, veReal size = 500.0f);
	virtual veAnimationPlayer* createAnimationPlayer(const std::string &name, veAnimationContainer *container);
	virtual veTexture* createTexture(const std::string &name, veTexture::TextureType texType = veTexture::TEXTURE_2D);
	virtual veMaterialArray* createMaterialArray(const std::string &name);
	virtual vePostProcesser* createPostProcesser(const std::string &name);
    virtual veParticleSystem* createParticleSystem(const std::string &name);
	virtual void removePostProcesser(const std::string &name);

	virtual void requestRender(veNode *node) = 0;
	virtual void requestRayCast(veRay *ray) = 0;
	virtual bool isNodeVisibleInScene(veNode *node) { return true; }

	const veCameraList& getCameraList() const { return _cameraList; }
	const veLightListMap& getLightListMap() const { return _lightListMap; }
	const vePostProcesserList& getPostProcesserList() const { return _postProcesserList; }
	void setSkyBox(veSkyBox *skybox) { _skyBox = skybox; needReload(); }
	veSkyBox* getSkyBox() const { return _skyBox.get(); }
	void setAmbientColor(const veVec3 &color) { _ambient = color; }
	const veVec3& getAmbientColor() const { return _ambient; }

	veBaseManager* getManager(const std::string &mgType);

	veNode* getRootNode() { return _root.get(); }
	void setDeltaTime(double deltaTime);
	double getDeltaTime() { return _deltaTime; }
	double getSimulationTime() { return _simulationTime; }

	void setCamera(veCamera *camera) { _mainCamera = camera; }
	veCamera* getCamera() { return _mainCamera.get(); }

	veRenderPipeline* getRenderPipeline() const { return _renderPipeline.get(); }

	void needReload();
	bool isNeedReload() { return _needReload; }

	void dispatchEvents(veEvent &event);
	bool simulation();

	void startThreading();
	void stopThreading();
	void enqueueTaskToThread(const veThreadPool::TaskCallBack& callback, void* callbackParam, const std::function<void()> &func);

	void addComponent(veComponent *component);
	void removeComponent(veComponent *component);

protected:

	virtual void update();
	virtual void render() = 0;
	void postRenderHandle();
	void resourceRecovery();
	void handleRequests();
	void enqueueRequest(const std::function<void()> &func);

protected:

	VE_Ptr<veNode>              _root;
	VE_Ptr<veSkyBox>            _skyBox;
	veCameraList                _cameraList;
	veLightListMap              _lightListMap;
	vePostProcesserList         _postProcesserList;
    veParticleSystemList        _particleSystemList;
	VE_Ptr<veFrameBufferObject> _postProcesserFBO;
	veVec3                      _ambient;
	VE_Ptr<veRenderPipeline>    _renderPipeline;

	VE_Ptr<veCamera>                                 _mainCamera;
	std::unordered_map<std::string, veBaseManager *> _managerList;

	veComponentList _componentList;

	veThreadPool                         _threadPool;
	std::mutex                           _requestQueueMutex;
	veLoopQueue< std::function<void()> > _requestQueue;

	std::mutex  _updatingMutex;
	std::mutex  _renderingMutex;
	std::condition_variable _renderingCondition;
	std::thread _renderingThread;
	bool        _stopThreading;

	bool _needReload;

	double _deltaTime;
	double _simulationTime;
	double _latestResourceRecoveredTime;
};

#endif