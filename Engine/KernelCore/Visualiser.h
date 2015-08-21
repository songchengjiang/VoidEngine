#ifndef _VE_VISUALISER_
#define _VE_VISUALISER_
#include <string>
#include "Prerequisites.h"
#include "BaseCore/Vector4.h"
#include "VE_Ptr.h"
#include "Registrar.h"
#include "Event.h"
#include "Node.h"
#include "RenderQueue.h"

class VE_EXPORT veVisualiser
{
	friend class veDirector;
public:
	~veVisualiser();

	USE_VE_PTR;

	int width();
	int height();
	void setClearColor(const veVec4 &color) { _clearColor = color; }
	const veVec4& getClearColor() const { return _clearColor; }
	void setClearMask(unsigned int mask) { _clearMask = mask; }
	unsigned int getClearMask() const { return _clearMask; }

	void setSceneNode(veNode *node);
	veNode* getSceneNode() const { return _root.get(); }

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
	veVec4 _clearColor;
	unsigned int _clearMask;
	GLFWwindow *_hwnd;
	VE_Ptr<veNode> _root;
	double _deltaTime;

	veRenderQueue _renderQueue;
};

typedef veRegistrar<GLFWwindow*, veVisualiser> veVisualiserRegistrar;

#endif