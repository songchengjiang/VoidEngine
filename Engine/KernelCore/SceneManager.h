#ifndef _VE_SCENE_MANAGER_
#define _VE_SCENE_MANAGER_
#include "Prerequisites.h"
#include "VE_Ptr.h"
#include "Visualiser.h"
#include "RenderQueue.h"
#include "ThreadPool.h"
#include <unordered_map>

class veEvent;
class veNode;
class veCamera;
class veLight;
class veSurface;
class veEntity;
class veSkyBox;
class veFont;
class veText;
class veImage;
class veAnimation;
class veAnimationContainer;
class veAnimationPlayer;
class veBaseManager;
class veRay;
typedef std::vector< veNode* > NodeList;
typedef std::vector< veCamera* > CameraList;
typedef std::vector< veLight* > LightList;
class VE_EXPORT veSceneManager
{
public:
	veSceneManager();
	~veSceneManager();

	USE_VE_PTR;

	veVisualiser* createVisualiser(int w, int h, const std::string &title);
	veVisualiser* getVisualiser() { return _visualiser.get(); }
	virtual veNode* createNode(const std::string &name) = 0;
	virtual veLight* createLight(const std::string &type, const std::string &name);
	virtual veSurface* createSurface(const std::string &name);
	virtual veImage* createImage(const std::string &name, veTexture *texture);
	virtual veText* createText(const std::string &name, veFont *font, const std::string &content = "");
	virtual veAnimation* createAnimation(const std::string &name);
	virtual veAnimationContainer* createAnimationContainer(const std::string &name);

	virtual veCamera* createCamera(const std::string &name, const veViewport &vp = { 0, 0, 0, 0 }) = 0;
	virtual veEntity* createEntity(const std::string &name);
	virtual veSkyBox* createSkyBox(const std::string &name, veReal size = 500.0f);
	virtual veAnimationPlayer* createAnimationPlayer(const std::string &name, veAnimationContainer *container);
	virtual veTexture* createTexture(const std::string &name, veTexture::TextureType texType = veTexture::TEXTURE_2D);

	virtual void requestRender(veNode *node) = 0;
	virtual void requestRayCast(veRay *ray) = 0;
	virtual bool isNodeVisibleInScene(veNode *node) { return true; }

	const NodeList& getNodeList() const { return _nodeList; }
	const CameraList& getCameraList() const { return _cameraList; }
	const LightList& getLightList() const { return _lightList; }

	void loadLightConfiguration(const std::string &filePath);

	veBaseManager* getManager(const std::string &mgType);

	veNode* getRootNode() { return _root.get(); }
	void setDeltaTime(double deltaTime) { _deltaTime = deltaTime; }
	double getDeltaTime() { return _deltaTime; }

	void dispatchEvents(veEvent &event);
	bool simulation();

	void startThreading();
	void stopThreading();

protected:

	virtual void update() = 0;
	virtual void render() = 0;

protected:

	VE_Ptr<veVisualiser> _visualiser;
	VE_Ptr<veNode> _root;
	NodeList _nodeList;
	CameraList _cameraList;
	LightList _lightList;

	std::unordered_map<std::string, veBaseManager *> _managerList;

	veThreadPool _threadPool;

	double _deltaTime;
};

#endif