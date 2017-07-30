#include "CameraManipulator.h"
#include "Node.h"
#include "SceneManager.h"
#include "Camera.h"
#include <algorithm>

veCameraManipulator::veCameraManipulator()
: _defaultCameraDistance(30.0f)
, _defaultCameraZoomScale(1.0f)
, _simulationTime(0.0)
, _latesTouchTime(0.0)
{
}

veCameraManipulator::~veCameraManipulator()
{

}

void veCameraManipulator::update(veSceneManager *sm)
{
    _simulationTime += sm->getDeltaTime();
}

bool veCameraManipulator::handle(veSceneManager *sm, veViewer *viewer, const veEvent &event)
{    
    if (event.getEventType() & veEvent::VE_MOUSE_EVENT) {
        if (event.getEventType() == veEvent::VE_PRESS) {
            _g1 = _g0 = veVec2(event.getMouseX(), event.getMouseY());
        }
        else if (event.getEventType() == veEvent::VE_DRAG) {
            if (event.getEventType() == veEvent::VE_DRAG) {
                _g1 = veVec2(event.getMouseX(), event.getMouseY());
                if (event.getMouseSymbol() == veEvent::VE_MOUSE_BUTTON_RIGHT) {
                    rotateCamera(_g0, _g1);
                }
                else if (event.getMouseSymbol() == veEvent::VE_MOUSE_BUTTON_MIDDLE) {
                    moveCamera(_g0, _g1);
                }
                _g0 = _g1;
            }
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
    else if (event.getEventType() & veEvent::VE_TOUCH_EVENT) {
        auto touchs = event.getTouches();
        if (event.getEventType() == veEvent::VE_TOUCH_MOVE) {
            if (touchs.size() == 1) {
                auto touch = touchs[0];
                _g0 = veVec2(touch.latestx, touch.latesty);
                _g1 = veVec2(touch.x, touch.y);
                rotateCamera(_g0, _g1);
            }
            else if (touchs.size() == 2) {
                veVec2 latest0 = veVec2(touchs[0].latestx, touchs[0].latesty);
                veVec2 latest1 = veVec2(touchs[1].latestx, touchs[1].latesty);
                veVec2 current0 = veVec2(touchs[0].x, touchs[0].y);
                veVec2 current1 = veVec2(touchs[1].x, touchs[1].y);
                veReal latestDelta = (latest0 - latest1).squaredLength();
                veReal currentDelta = (current0 - current1).squaredLength();
                //veLog("veMath::veAbs(latestDelta - currentDelta): %f", veMath::veAbs(latestDelta - currentDelta));
                veReal  absDis = veMath::veAbs(latestDelta - currentDelta);
                if (0.01 < absDis){
                    if (latestDelta < currentDelta) {
                        zoomCamera(-absDis * 10.0f);
                    }
                    else {
                        zoomCamera(absDis * 10.0f);
                    }
                }
                
                _g0 = (latest0 + latest1) * 0.5f;
                _g1 = (current0 + current1) * 0.5f;
                moveCamera(_g0, _g1);
            }
        }else if (event.getEventType() == veEvent::VE_TOUCH_END) {
            if (touchs.size() == 0) {
                if (_latesTouchTime != 0.0) {
                    if ((_simulationTime - _latesTouchTime) < 0.4) {
                        //resetCamera();
                    }
                    _latesTouchTime = 0.0;
                }
                else {
                    _latesTouchTime = _simulationTime;
                }
            }
        }
    }
    return false;
}

void veCameraManipulator::start(veSceneManager *sm)
{
    resetCamera();
}

void veCameraManipulator::rotateCamera(const veVec2 &g0, const veVec2 &g1) {
    if (g0 == g1) return;
    
    float angle;
    veVec3 axes;
    trackball(axes, angle, g0.x(), g0.y(), g1.x(), g1.y());
    _rotation = veQuat(angle, axes) * _rotation;
    updateViewMatrix();
}

void veCameraManipulator::moveCamera(const veVec2 &g0, const veVec2 &g1) {
    if (g0 == g1) return;
    veVec2 delta = g0 - g1;
    delta *= 10;
    _center += _rotation * veVec3(delta.x(), delta.y(), .0f);
    updateViewMatrix();
}

void veCameraManipulator::zoomCamera(float zoomDelta) {
    _translate.z() += zoomDelta * _defaultCameraZoomScale;
    updateViewMatrix();
}

void veCameraManipulator::resetCamera() {
    _translate = veVec3(0.0f, 0.0f, _defaultCameraDistance);
    _center = veVec3::ZERO;
    _rotation = veQuat::IDENTITY;
    updateViewMatrix();
}

void veCameraManipulator::updateViewMatrix()
{
    veMat4 center;
    center.makeTranslation(_center);
    veMat4 trans;
    trans.makeTranslation(_translate);
    _attachedNode->setMatrix(center * veMat4::rotation(_rotation) * trans);
}

void veCameraManipulator::trackball(veVec3 & axis, float & angle, float p1x, float p1y, float p2x, float p2y) {
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

float veCameraManipulator::tb_project_to_sphere(float r, float x, float y) {
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
