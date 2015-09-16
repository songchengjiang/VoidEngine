#ifndef _BASE_TEST_
#define _BASE_TEST_
#include "VoidEngine.h"

class CameraManipulator : public veInputer
{
public:
	CameraManipulator()
		: _camera(nullptr)
	{
		resetCamera();
	}
	~CameraManipulator(){}

	virtual bool handle(veNode *node, veVisualiser *vs, const veEvent &event) {
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
		_rotation = veQuat(angle * 3.0f, axes) * _rotation;
		updateViewMatrix();
	}

	void moveCamera(const veVec2 &g0, const veVec2 &g1) {
		if (g0 == g1) return;
		veVec2 delta = g0 - g1;
		delta *= 20.0f;
		_center += _rotation * veVec3(delta.x(), delta.y(), .0f);
		updateViewMatrix();
	}

	void zoomCamera(float zoomDelta) {
		_translate.z() += zoomDelta;
		updateViewMatrix();
	}

	void resetCamera() {
		_translate = veVec3(0.0f, 0.0f, 30.0f);
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
};

class BaseTest
{
public:
	BaseTest() {
		_visualiser = veDirector::instance()->createVisualiser(800, 600, "Game");
		_visualiser->getCamera()->setViewMatrixAslookAt(veVec3(0.0f, 0.0f, 30.0f), veVec3::ZERO, veVec3::UNIT_Y);
		_visualiser->getCamera()->addComponent(new CameraManipulator);
	};
	~BaseTest() {};

	int run() {
		return veDirector::instance()->run();
	}

protected:

	veVisualiser *_visualiser;
};

#endif