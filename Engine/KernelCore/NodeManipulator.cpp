#include "NodeManipulator.h"
#include "Node.h"
#include "SceneManager.h"
#include "Camera.h"
#include <algorithm>

veNodeManipulator::veNodeManipulator()
: _defaultCameraDistance(30.0f)
, _defaultCameraZoomScale(0.1f)
, _simulationTime(0.0)
, _latesTouchTime(0.0)
{
}

veNodeManipulator::~veNodeManipulator()
{

}

void veNodeManipulator::update(veSceneManager *sm)
{
    _simulationTime += sm->getDeltaTime();
}

bool veNodeManipulator::handle(veSceneManager *sm, veViewer *viewer, const veEvent &event)
{    
    if (event.getEventType() & veEvent::VE_MOUSE_EVENT) {
        if (event.getEventType() == veEvent::VE_PRESS) {
            _g1 = _g0 = veVec2(event.getMouseX(), event.getMouseY());
        }
        else if (event.getEventType() == veEvent::VE_DRAG) {
            if (event.getEventType() == veEvent::VE_DRAG) {
                _g1 = veVec2(event.getMouseX(), event.getMouseY());
                if (event.getMouseSymbol() == veEvent::VE_MOUSE_BUTTON_RIGHT) {
                    rotateNode(_g0, _g1);
                }
                _g0 = _g1;
            }
        }
        else if (event.getEventType() == veEvent::VE_SCROLL_UP) {
            zoomNode(-1.0f);
        }
        else if (event.getEventType() == veEvent::VE_SCROLL_DOWN) {
            zoomNode(1.0f);
        }
    }
    else if (event.getEventType() & veEvent::VE_TOUCH_EVENT) {
        auto touchs = event.getTouches();
        if (event.getEventType() == veEvent::VE_TOUCH_MOVE) {
            if (touchs.size() == 1) {
                auto touch = touchs[0];
                _g0 = veVec2(touch.latestx, touch.latesty);
                _g1 = veVec2(touch.x, touch.y);
                rotateNode(_g0, _g1);
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
                        zoomNode(-absDis);
                    }
                    else {
                        zoomNode(absDis);
                    }
                }
                
                _g0 = (latest0 + latest1) * 0.5f;
                _g1 = (current0 + current1) * 0.5f;
                //moveNode(_g0, _g1);
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

void veNodeManipulator::start(veSceneManager *sm)
{
    _attachedNode->getMatrix().decomposition(&_translation, &_scale, &_rotation);
}

void veNodeManipulator::rotateNode(const veVec2 &g0, const veVec2 &g1) {
    if (g0 == g1) return;
    
    float angle;
    veVec3 axes;
    trackball(axes, angle, g0.x(), g0.y(), g1.x(), g1.y());
    veQuat rot = veQuat(angle, axes);
    rot.inverse();
    _rotation = rot * _rotation;
    updateMatrix();
}

void veNodeManipulator::zoomNode(float zoomDelta) {
    _scale.x() -= zoomDelta * _defaultCameraZoomScale;
    _scale.y() = _scale.z() = _scale.x();
    updateMatrix();
}

void veNodeManipulator::moveNode(const veVec2 &g0, const veVec2 &g1)
{
    if (g0 == g1) return;
    veVec2 delta = g1 - g0;
    _translation += _rotation * veVec3(delta.x(), 0.0f, -delta.y());
    updateMatrix();
}

void veNodeManipulator::updateMatrix()
{
    _attachedNode->setMatrix(veMat4::transform(_translation, _scale, _rotation));
}

void veNodeManipulator::trackball(veVec3 & axis, float & angle, float p1x, float p1y, float p2x, float p2y) {
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

float veNodeManipulator::tb_project_to_sphere(float r, float x, float y) {
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
