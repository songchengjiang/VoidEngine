#ifndef _VE_SCENE_MANAGER_
#define _VE_SCENE_MANAGER_
#include "Prerequisites.h"
#include "VE_Ptr.h"
#include "Visualiser.h"
#include "RenderQueue.h"
#include <unordered_map>

class veEvent;
class veNode;
class veCamera;
class veLight;
class veOverlay;
class veEntity;
class veFont;
class veText;
class veAnimation;
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

	veVisualiser* createVisualiser(int w, int h, const std::string &title);
	veVisualiser* getVisualiser() { return _visualiser.get(); }
	virtual veNode* createNode() = 0;
	virtual veOverlay* createOverlay();
	virtual veCamera* createCamera(const veViewport &vp = { 0, 0, 0, 0 }) = 0;
	virtual veLight* createLight(const std::string &type);
	virtual veEntity* createEntity(const std::string &name);
	virtual veText* createText(veFont *font, const std::string &content = "");
	virtual veAnimation* createAnimation();

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

protected:

	virtual void update() = 0;
	virtual void render() = 0;

protected:

	veRenderQueue *_renderQueue;
	VE_Ptr<veVisualiser> _visualiser;
	VE_Ptr<veNode> _root;
	NodeList _nodeList;
	CameraList _cameraList;
	LightList _lightList;

	std::unordered_map<std::string, veBaseManager *> _managerList;

	double _deltaTime;
};

#endif