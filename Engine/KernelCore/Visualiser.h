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
	~veVisualiser();

	USE_VE_PTR;

	int width();
	int height();

	void setCamera(veCamera *camera) { _mainCamera = camera; }
	veCamera* getCamera() { return _mainCamera.get(); }
	//int addCamera(veCamera *camera);
	//veCamera* getCamera(unsigned int idx);
	//veCamera* removeCamera(unsigned int idx);
	//unsigned int getCameraNum() const { return _cameras.size(); }

private:

	veVisualiser(int w, int h, const std::string &title);

private:

	int _width;
	int _height;
	std::string _title;
	GLFWwindow *_hwnd;
	VE_Ptr<veCamera> _mainCamera;

	veSceneManager *_sceneManager;
};

typedef veRegistrar<GLFWwindow*, veVisualiser> veVisualiserRegistrar;

#endif