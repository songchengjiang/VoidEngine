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
#include "light.h"
#include "RenderQueue.h"

class veSceneManager;
class VE_EXPORT veVisualiser
{
	friend class veSceneManager;
	friend class veCamera;
public:
	virtual ~veVisualiser();

	USE_VE_PTR;

	int width() { return _width; }
	int height() { return _height; }

	void setCamera(veCamera *camera) { _mainCamera = camera; }
	veCamera* getCamera() { return _mainCamera.get(); }

	virtual bool dispatchEvents() = 0;

protected:

	veVisualiser(int w, int h, const std::string &title);
	virtual void setContextCurrent() = 0;
	virtual void swapBuffers() = 0;
	virtual bool isWindowShouldClose() = 0;

protected:

	int _width;
	int _height;
	std::string _title;
	bool        _isInited;
	VE_Ptr<veCamera> _mainCamera;

	Events  _events;
	veEvent _currentEvent;

	veSceneManager *_sceneManager;
};

#endif