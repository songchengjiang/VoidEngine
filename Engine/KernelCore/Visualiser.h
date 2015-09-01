#ifndef _VE_VISUALISER_
#define _VE_VISUALISER_
#include <string>
#include "Prerequisites.h"
#include "BaseCore/Vector4.h"
#include "VE_Ptr.h"
#include "Registrar.h"
#include "Event.h"
#include "Node.h"
#include "Camera.h"
#include "RenderQueue.h"

class VE_EXPORT veVisualiser
{
	friend class veDirector;
public:
	~veVisualiser();

	USE_VE_PTR;

	int width();
	int height();

	void setSceneNode(veNode *node);
	veNode* getSceneNode() const { return _root.get(); }

	int addCamera(veCamera *camera);
	veCamera* getCamera(unsigned int idx);
	veCamera* removeCamera(unsigned int idx);
	unsigned int getCameraNum() const { return _cameras.size(); }

	veRenderQueue& getRenderQueue() { return _renderQueue; }

	bool simulate(double deltaTime);
	bool dispatchEvent(double deltaTime, const veEvent &event);
	double getDeltaTime() const { return _deltaTime; }

private:

	veVisualiser(int w, int h, const std::string &title);

	void update();
	void render();

private:

	int _width;
	int _height;
	std::string _title;
	GLFWwindow *_hwnd;
	VE_Ptr<veNode> _root;
	std::vector< VE_Ptr<veCamera> > _cameras;
	double _deltaTime;

	veRenderQueue _renderQueue;
};

typedef veRegistrar<GLFWwindow*, veVisualiser> veVisualiserRegistrar;

#endif