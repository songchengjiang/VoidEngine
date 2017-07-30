#ifndef _BASE_TEST_
#define _BASE_TEST_
#include "VoidEngine.h"

#if defined(_MSC_VER)
#pragma warning( disable : 4996 )
#endif

class LightUpdater : public veComponent
{
public:

	LightUpdater(veLight *light, float radius, float height)
		: _lastChangeColorTime(0.0)
        , _light(light){
		_angle = veMath::veRandomUnitization() * veMath::TWO_PI;
		_radius = radius;
		_height = height;
		_oriColor = _desColor = veVec3(veMath::veRandomUnitization(), veMath::veRandomUnitization(), veMath::veRandomUnitization());
	}
	virtual bool handle(veSceneManager *sm, veViewer *viewer, const veEvent &event) override{
		return false;
	}

	virtual void update(veSceneManager *sm) override{
        updateColor(sm->getDeltaTime());
        updateMatrix(sm->getDeltaTime());
	}

private:

	void updateColor(double deltaTime) {
		veVec3 col = _oriColor * (1.0 - _lastChangeColorTime) + _desColor * _lastChangeColorTime;
		_light->setColor(col);

		if (1.0 < _lastChangeColorTime) {
			_oriColor = _desColor;
			_desColor = veVec3(veMath::veRandomUnitization(), veMath::veRandomUnitization(), veMath::veRandomUnitization());
			_lastChangeColorTime = 0.0f;
		}
		_lastChangeColorTime += deltaTime;
	}

	void updateMatrix(double deltaTime) {
		veReal x = _radius * veMath::veCos(_angle);
		veReal y = _radius * veMath::veSin(_angle);
		_light->getAttachedNode()->setMatrix(veMat4::lookAt(veVec3(x, _height, y), veVec3::ZERO, veVec3::UNIT_Y));
		_angle += veMath::QUARTER_PI * deltaTime;
		_angle = fmod(_angle, veMath::TWO_PI);
	}

private:

	double _lastChangeColorTime;
	veVec3 _oriColor;
	veVec3 _desColor;
	veReal _angle;
	veReal _radius;
	veReal _height;
    veLight *_light;
};

class BaseTest
{
public:
	BaseTest() {
		veFile::instance()->addSearchPath("../resources/");
		_sceneManager = new veOctreeSceneManager(veBoundingBox(veVec3(-1000.0f), veVec3(1000.0f)), 8);
        _mainViewer = veApplication::instance()->getViewer(0);
        _mainViewer->setSceneManager(_sceneManager);
		int width = _mainViewer->width();
		int height = _mainViewer->height();
        _cameraNode = _sceneManager->createNode("MainCamera");
        _camera = _sceneManager->createCamera("MainCamera", {0, 0, width, height });
		_camera->setProjectionMatrixAsPerspective(60.0f, (float)width / (float)height, 0.1f, 1000.0f);
        _cameraNode->addComponent(_camera);
        _cameraNode->setMatrix(veMat4::lookAt(veVec3(0.0f, 0.0f, 30.0f), veVec3::ZERO, veVec3::UNIT_Y));
		_defaultCameraDistance = 30.0f;
		_defaultCameraZoomScale = 1.0f;

		veVec4 pos = _camera->projectionMatrix() * _camera->viewMatrix() * veVec4(10.0, 10.0, 10.0, 1.0);

	};
	~BaseTest() {
		VE_SAFE_DELETE(_sceneManager);
	};

	virtual void init() {
        _cameraNode->addComponent(new veCameraManipulator());
        _sceneManager->getRootNode()->addChild(_cameraNode);
	}

protected:

	veSceneManager *_sceneManager;
    veNode    *_cameraNode;
	veCamera  *_camera;
    veViewer  *_mainViewer;
	veReal _defaultCameraDistance;
	veReal _defaultCameraZoomScale;
};

#endif
