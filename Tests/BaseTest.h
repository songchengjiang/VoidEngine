#ifndef _BASE_TEST_
#define _BASE_TEST_
#include "VoidEngine.h"

#if defined(_MSC_VER)
#pragma warning( disable : 4996 )
#endif

class LightUpdater : public veComponent
{
public:

	LightUpdater()
		: _lastChangeColorTime(0.0) {
		_angle = veMath::randomUnitization() * veMath::TWO_PI;
		_radius = 10.0f * veMath::randomUnitization() + 2.0f;
		_height = 10.0f * (veMath::randomUnitization());
		_oriColor = _desColor = veVec3(veMath::randomUnitization(), veMath::randomUnitization(), veMath::randomUnitization());
	}
	virtual bool handle(veNode *node, veSceneManager *sm, const veEvent &event) override{
		return false;
	}

	virtual void update(veNode *node, veSceneManager *sm) override{
		auto light = static_cast<veLight *>(node);
		if (light) {
			auto param = light->getParameter("color");
			updateColor(param, sm->getDeltaTime());
			updateMatrix(light, sm->getDeltaTime());
		}
	}

private:

	void updateColor(veParameter *colorParam, double deltaTime) {
		veVec3 col = _oriColor * (1.0 - _lastChangeColorTime) + _desColor * _lastChangeColorTime;
		colorParam->set(col);

		if (1.0 < _lastChangeColorTime) {
			_oriColor = _desColor;
			_desColor = veVec3(veMath::randomUnitization(), veMath::randomUnitization(), veMath::randomUnitization());
			_lastChangeColorTime = 0.0f;
		}
		_lastChangeColorTime += deltaTime;
	}

	void updateMatrix(veLight *light, double deltaTime) {
		veReal x = _radius * veMath::cos(_angle);
		veReal y = _radius * veMath::sin(_angle);
		light->setMatrix(veMat4::lookAt(veVec3(x, y, _height), veVec3::ZERO, veVec3::UNIT_Y));
		_angle += veMath::QUARTER_PI * deltaTime;
	}

private:

	double _lastChangeColorTime;
	veVec3 _oriColor;
	veVec3 _desColor;
	veReal _angle;
	veReal _radius;
	veReal _height;
};

class CameraManipulator : public veComponent
{
public:
	CameraManipulator(veReal defaultCameraDistance, veReal defaultCameraZoomScale)
		: _camera(nullptr)
		, _defaultCameraDistance(defaultCameraDistance)
		, _defaultCameraZoomScale(defaultCameraZoomScale)
	{
		resetCamera();
	}
	~CameraManipulator(){}

	virtual bool handle(veNode *node, veSceneManager *sm, const veEvent &event) override{
		_camera = static_cast<veCamera *>(node);
		if (event.getEventType() & veEvent::VE_MOUSE_EVENT) {
			if (event.getEventType() == veEvent::VE_PRESS) {
				_g1 = _g0 = veVec2(event.getMouseX(), event.getMouseY());
			}
			else if (event.getEventType() == veEvent::VE_MOVE) {
				_g1 = veVec2(event.getMouseX(), event.getMouseY());
				if (event.getMouseSymbol() == veEvent::VE_MOUSE_BUTTON_LEFT) {
					rotateCamera(_g0, _g1);
				}
				else if (event.getMouseSymbol() == veEvent::VE_MOUSE_BUTTON_MIDDLE) {
					moveCamera(_g0, _g1);
				}
				_g0 = _g1;
			}
			else if (event.getEventType() == veEvent::VE_SCROLL_UP) {
				zoomCamera(-1.0f);
			}
			else if (event.getEventType() == veEvent::VE_SCROLL_DOWN) {
				zoomCamera(1.0f);
			}
		}
		else if (event.getEventType() & veEvent::VE_KEYBOARD_EVENT) {
			if (event.getKeySymbol() == veEvent::VE_KEY_SPACE) {
				resetCamera();
			}
		}
		return false;
	}

private:

	void rotateCamera(const veVec2 &g0, const veVec2 &g1) {
		if (g0 == g1) return;

		float angle;
		veVec3 axes;
		trackball(axes, angle, g0.x(), g0.y(), g1.x(), g1.y());
		_rotation = veQuat(angle, axes) * _rotation;
		updateViewMatrix();
	}

	void moveCamera(const veVec2 &g0, const veVec2 &g1) {
		if (g0 == g1) return;
		veVec2 delta = g0 - g1;
		delta *= 10;
		_center += _rotation * veVec3(delta.x(), delta.y(), .0f);
		updateViewMatrix();
	}

	void zoomCamera(float zoomDelta) {
		_translate.z() += zoomDelta * _defaultCameraZoomScale;
		updateViewMatrix();
	}

	void resetCamera() {
		_translate = veVec3(0.0f, 0.0f, _defaultCameraDistance);
		_center = veVec3::ZERO;
		_rotation = veQuat::IDENTITY;
		updateViewMatrix();
	}

	void updateViewMatrix()
	{
		veMat4 center;
		center.makeTranslation(_center);
		veMat4 trans;
		trans.makeTranslation(_translate);

		if (_camera)
			_camera->setMatrix(center * veMat4::rotation(_rotation) * trans);
	}

	void trackball(veVec3 & axis, float & angle, float p1x, float p1y, float p2x, float p2y) {
		float trackballSize = 1.0f;

		veVec3 uv = _rotation * veVec3(0.0f, 1.0f, 0.0f);
		veVec3 sv = _rotation * veVec3(1.0f, 0.0f, 0.0f);
		veVec3 lv = _rotation * veVec3(0.0f, 0.0f, -1.0f);

		veVec3 p1 = sv * p1x + uv * p1y - lv * tb_project_to_sphere(trackballSize, p1x, p1y);
		veVec3 p2 = sv * p2x + uv * p2y - lv * tb_project_to_sphere(trackballSize, p2x, p2y);

		axis = p2.crossProduct(p1);
		axis.normalize();

		float t = (p2 - p1).length() / (2.0f * trackballSize);

		if (t > 1.0f) t = 1.0f;
		if (t < -1.0f) t = -1.0f;
		angle = asin(t);
	}

	float tb_project_to_sphere(float r, float x, float y) {
		float d, t, z;
		d = sqrt(x*x + y*y);
		if (d < r * 0.70710678118654752440f)
		{
			z = sqrt(r*r - d*d);
		}
		else
		{
			t = r / 1.41421356237309504880f;
			z = t*t / d;
		}
		return z;
	}


private:

	veQuat _rotation;
	veVec3 _center;
	veVec3 _translate;
	veCamera *_camera;
	veVec2 _g0;
	veVec2 _g1;
	veReal _defaultCameraDistance;
	veReal _defaultCameraZoomScale;
};

class BaseTest
{
public:
	BaseTest() {
		_sceneManager = new veOctreeSceneManager(veBoundingBox(veVec3(-1000.0f), veVec3(1000.0f)), 8);
		veDirector::instance()->setSceneManager(_sceneManager);
		int width = 800;
		int height = 600;
		_sceneManager->createVisualiser(width, height, "VoidEngine");
		_camera = _sceneManager->createCamera("MainCamera", {0, 0, width, height });
		_camera->setProjectionMatrixAsPerspective(30.0f, (float)width / (float)height, 1.0f, 1000.0f);
		_camera->setViewMatrixAslookAt(veVec3(0.0f, 0.0f, 30.0f), veVec3::ZERO, veVec3::UNIT_Y);
		_sceneManager->getRootNode()->addChild(_camera);
		_sceneManager->getVisualiser()->setCamera(_camera);
		_defaultCameraDistance = 30.0f;
		_defaultCameraZoomScale = 1.0f;
	};
	~BaseTest() {
		VE_SAFE_DELETE(_sceneManager);
	};

	virtual void init() {
	}

	int run() {
		init();
		_camera->addComponent(new CameraManipulator(_defaultCameraDistance, _defaultCameraZoomScale));
		return veDirector::instance()->run();
	}

protected:

	veSceneManager *_sceneManager;
	veCamera  *_camera;
	veReal _defaultCameraDistance;
	veReal _defaultCameraZoomScale;
};

#endif