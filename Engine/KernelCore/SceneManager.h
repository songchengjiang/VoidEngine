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

#include "RenderPipeline.h"
#include "Component.h"
#include "EffectCore/ParticleSystem.h"

#include <unordered_map>

class veEvent;
class veNode;
class veCamera;
class veLight;
class veSurface;
class veMesh;
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
class veViewer;
class veEventDispatcher;
class veFrameBufferObject;
class veTerrain;

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
    
    virtual veFrameBufferObject* createFrameBufferObject(const std::string &name);

	virtual veRay* createRay(const veVec3 &start, const veVec3 &end);
	virtual veCamera* createCamera(const std::string &name, const veViewport &vp = { 0, 0, 0, 0 }) = 0;
	virtual veMesh* createMesh(const std::string &name);
	virtual veSphere* createSphere(const std::string &name);
	virtual veCone* createCone(const std::string &name);
	virtual veSkyBox* createSkyBox(const std::string &name, veReal size = 500.0f);
	virtual veAnimationPlayer* createAnimationPlayer(const std::string &name, veAnimationContainer *container);
	virtual veTexture* createTexture(const std::string &name, veTexture::TextureType texType = veTexture::TEXTURE_2D);
	virtual veMaterialArray* createMaterialArray(const std::string &name);
	virtual vePostProcesser* createPostProcesser(const std::string &name);
    virtual veParticleSystem* createParticleSystem(const std::string &name);

	virtual void requestRender(veNode *node) = 0;
	virtual void requestRayCast(veRay *ray) = 0;
	virtual bool isNodeVisibleInScene(veNode *node) { return true; }

	const veCameraList& getCameraList() const { return _cameraList; }
	const veLightListMap& getLightListMap() const { return _lightListMap; }
	void setSkyBox(veSkyBox *skybox) { _skyBox = skybox; }
	veSkyBox* getSkyBox() const { return _skyBox.get(); }
	void setAmbientColor(const veVec3 &color) { _ambient = color; }
	const veVec3& getAmbientColor() const { return _ambient; }

	veBaseManager* getManager(const std::string &mgType);

	veNode* getRootNode() { return _root.get(); }
    void setDeltaTime(double deltaTime) { _deltaTime = deltaTime; }
	double getDeltaTime() { return _deltaTime; }
	double getSimulationTime() { return _simulationTime; }

	veRenderPipeline* getRenderPipeline() const { return _renderPipeline.get(); }
    
    veRenderState* getRenderState(unsigned int contextID) { return &_renderStateList[contextID]; }

    void setResourceRecoveredIntervalTime(double time) { _resourceRecoveredIntervalTime = time; }

    void event(veViewer *viewer);
	void update(veViewer *viewer);
    void render(veViewer *viewer);

	void enqueueTaskToThread(const std::function<void()> &func);
    void enqueueRequestToRenderThread(const std::function<void()> &func);

	void addComponent(veComponent *component);
	void removeComponent(veComponent *component);
	const veComponentList& getComponentList() const { return _componentList; }
    
    void attachViewer(veViewer *viewer);
    void detachViewer(veViewer *viewer);

protected:

	virtual void updateImp() = 0;
	virtual void renderImp(veViewer *viewer) = 0;
    void startThreading();
    void stopThreading();
    void dispatchEvents(veViewer *viewer);
    void handleEvent(veViewer *viewer, const veEvent &event);
	void postRenderHandle(veViewer *viewer);
	void resourceRecovery(veViewer *viewer);
	void handleRequests();

protected:

    veComponentList             _componentList;
	VE_Ptr<veNode>              _root;
	VE_Ptr<veSkyBox>            _skyBox;
	veCameraList                _cameraList;
	veLightListMap              _lightListMap;
    veParticleSystemList        _particleSystemList;
	veVec3                      _ambient;
	VE_Ptr<veRenderPipeline>    _renderPipeline;
    std::vector<veViewer *>     _attachedViewers;

	std::unordered_map<std::string, veBaseManager *> _managerList;
    
    std::unordered_map<unsigned int, veRenderState> _renderStateList;

	veThreadPool                         _threadPool;
	std::mutex                           _requestQueueMutex;
	veLoopQueue< std::function<void()> > _requestQueue;

	std::mutex  _updatingMutex;
	std::mutex  _renderingMutex;
	std::condition_variable _renderingCondition;
	std::thread _renderingThread;
	bool        _stopThreading;


	double _deltaTime;
	double _simulationTime;
    double _resourceRecoveredIntervalTime;
	double _latestResourceRecoveredTime;
};

#endif