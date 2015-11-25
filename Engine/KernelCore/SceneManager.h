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

#include <unordered_map>

class veEvent;
class veNode;
class veCamera;
class veLight;
class veSurface;
class veEntity;
class veFont;
class veText;
class veImage;
class veAnimation;
class veAnimationContainer;
class veAnimationPlayer;
class veBaseManager;
class veRay;

class VE_EXPORT veSceneManager
{
public:
	veSceneManager();
	virtual ~veSceneManager();

	USE_VE_PTR;

	virtual veNode* createNode(const std::string &name) = 0;
	virtual veLight* createLight(const std::string &type, const std::string &name);
	virtual veSurface* createSurface(const std::string &name);
	virtual veImage* createImage(const std::string &name, veTexture *texture);
	virtual veText* createText(const std::string &name, veFont *font, const std::string &content = "");
	virtual veAnimation* createAnimation(const std::string &name);
	virtual veAnimationContainer* createAnimationContainer(const std::string &name);

	virtual veRay* createRay(const veVec3 &start, const veVec3 &end);
	virtual veCamera* createCamera(const std::string &name, const veViewport &vp = { 0, 0, 0, 0 }) = 0;
	virtual veEntity* createEntity(const std::string &name);
	virtual veSkyBox* createSkyBox(const std::string &name, veReal size = 500.0f);
	virtual veAnimationPlayer* createAnimationPlayer(const std::string &name, veAnimationContainer *container);
	virtual veTexture* createTexture(const std::string &name, veTexture::TextureType texType = veTexture::TEXTURE_2D);
	virtual vePostProcesser* createPostProcesser(const std::string &name);

	virtual void requestRender(veNode *node) = 0;
	virtual void requestRayCast(veRay *ray) = 0;
	virtual bool isNodeVisibleInScene(veNode *node) { return true; }

	const veNodeList& getNodeList() const { return _nodeList; }
	const veCameraList& getCameraList() const { return _cameraList; }
	const veLightList& getLightList() const { return _lightList; }
	const veSkyBox* getSkyBox() const { return _skyBox.get(); }

	veBaseManager* getManager(const std::string &mgType);

	veNode* getRootNode() { return _root.get(); }
	void setDeltaTime(double deltaTime);
	double getDeltaTime() { return _deltaTime; }
	double getSimulationTime() { return _simulationTime; }

	void setCamera(veCamera *camera) { _mainCamera = camera; }
	veCamera* getCamera() { return _mainCamera.get(); }

	void reload();
	bool needReload() { return _needReload; }

	void dispatchEvents(veEvent &event);
	bool simulation();

	void startThreading();
	void stopThreading();

protected:

	virtual void update() = 0;
	virtual void render() = 0;
	void handleRequests();
	void enqueueRequest(const std::function<void()> &func);

protected:

	VE_Ptr<veNode> _root;
	veNodeList _nodeList;
	veCameraList _cameraList;
	veLightList _lightList;
	veRayList   _rayList;
	VE_Ptr<veSkyBox> _skyBox;
	vePostProcesserList _postProcesserList;
	VE_Ptr<veFrameBufferObject> _postProcesserFBO;

	VE_Ptr<veCamera> _mainCamera;
	std::unordered_map<std::string, veBaseManager *> _managerList;

	veThreadPool _threadPool;
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
};

#endif