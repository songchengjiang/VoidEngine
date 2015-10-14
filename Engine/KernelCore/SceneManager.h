#ifndef _VE_SCENE_MANAGER_
#define _VE_SCENE_MANAGER_
#include "Prerequisites.h"
#include "VE_Ptr.h"
#include "Visualiser.h"
#include "RenderQueue.h"

class veEvent;
class veNode;
class veCamera;
class veLight;
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
	virtual veNode* createNode(const std::string &filePath) = 0;
	virtual veCamera* createCamera(const veViewport &vp = {0, 0, 0, 0}) = 0;
	virtual veCamera* createCamera(const std::string &filePath) = 0;
	virtual veLight* createLight(const std::string &className) = 0;
	//virtual veLight* createLight(const std::string &filePath) = 0;

	const NodeList& getNodeList() const { return _nodeList; }
	const CameraList& getCameraList() const { return _cameraList; }
	const LightList& getLightList() const { return _lightList; }

	void dispatchEvents(double deltaTime, veEvent &event);
	bool simulation(double deltaTime);

	veNode* getRootNode() { return _root.get(); }

	double getDeltaTime() { return _deltaTime; }

protected:

	virtual void update() = 0;
	virtual void cull() = 0;
	virtual void render() = 0;

protected:

	veRenderQueue _renderQueue;
	VE_Ptr<veVisualiser> _visualiser;
	VE_Ptr<veNode> _root;
	NodeList _nodeList;
	CameraList _cameraList;
	LightList _lightList;

	double _deltaTime;
};

#endif