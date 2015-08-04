#ifndef _VE_VISUALISER_
#define _VE_VISUALISER_
#include <string>
#include "Prerequisites.h"
#include "BaseCore/Vector4.h"
#include "VE_Ptr.h"
#include "Registrar.h"
#include "Event.h"
#include "Node.h"

class VE_EXPORT veVisualiser
{
	friend class veDirector;
public:
	~veVisualiser();

	int width() const { return _width; }
	int height() const { return _height; }
	void setClearColor(const veVec4 &color) { _clearColor = color; }
	const veVec4& getClearColor() const { return _clearColor; }
	void setClearMask(unsigned int mask) { _clearMask = mask; }
	unsigned int getClearMask() const { return _clearMask; }

	void setSceneNode(veNode *node);
	veNode* getSceneNode() const { return _root.get(); }

	bool simulate(double deltaTime);
	bool dispatchEvent(const veEvent &event);

private:

	veVisualiser(int w, int h, const std::string &title);

	void update(double deltaTime);
	void render(double deltaTime);

private:

	int _width;
	int _height;
	std::string _title;
	veVec4 _clearColor;
	unsigned int _clearMask;
	GLFWwindow *_hwnd;
	VE_Ptr<veNode> _root;
};

typedef veRegistrar<GLFWwindow*, veVisualiser> veVisualiserRegistrar;

#endif