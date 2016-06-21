#include "GizmoTranslation.h"
#include "KernelCore/SceneManager.h"
#include "GizmoRenderer.h"

#define GIZMO_PLANE_SIZE 0.25f

veGizmoTranslation::veGizmoTranslation(veSceneManager *sm)
    : veGizmo(sm)
{
    drawAxes(veVec4(1.0f, 0.0f, 0.0f, 1.0f), veVec4(0.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 0.0f, 1.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(0.0f, 0.0f, 1.0f, 0.2f));
}

veGizmoTranslation::~veGizmoTranslation()
{

}

veGizmoTranslation::AxesType veGizmoTranslation::touchDown(const veVec2 &screenCoords){
    veVec3 start = _sceneManager->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, -1.0f);
    veVec3 end = _sceneManager->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, 1.0f);
    veMat4 wton = _parents[0]->getWorldToNodeMatrix();
    _ray->setStart(wton * start);
    _ray->setEnd(wton * end);
    
    AxesType at = AxesType::AT_NONE;
    if (_ray->isIntersectWith(veVec3(0.0f, 0.0f, GIZMO_PLANE_SIZE) * _scale, veVec3::ZERO, veVec3(0.0f, GIZMO_PLANE_SIZE, 0.0f) * _scale, nullptr, nullptr) ||
        _ray->isIntersectWith(veVec3(0.0f, 0.0f, GIZMO_PLANE_SIZE) * _scale, veVec3(0.0f, GIZMO_PLANE_SIZE, 0.0f) * _scale, veVec3(0.0f, GIZMO_PLANE_SIZE, GIZMO_PLANE_SIZE) * _scale, nullptr, nullptr)){
        drawAxes(veVec4(1.0f, 0.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 0.5f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(0.0f, 0.0f, 1.0f, 0.2f));
        _planeNormal = veVec3::UNIT_X;
        at = AxesType::AT_YZ;
    }else if (_ray->isIntersectWith(veVec3::ZERO, veVec3(0.0f, 0.0f, GIZMO_PLANE_SIZE) * _scale, veVec3(GIZMO_PLANE_SIZE, 0.0f, GIZMO_PLANE_SIZE) * _scale, nullptr, nullptr) ||
              _ray->isIntersectWith(veVec3::ZERO, veVec3(GIZMO_PLANE_SIZE, 0.0f, GIZMO_PLANE_SIZE) * _scale, veVec3(GIZMO_PLANE_SIZE, 0.0f, 0.0f) * _scale, nullptr, nullptr)){
        drawAxes(veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(1.0f, 1.0f, 0.0f, 0.5f), veVec4(0.0f, 0.0f, 1.0f, 0.2f));
        _planeNormal = veVec3::UNIT_Y;
        at = AxesType::AT_XZ;
    }else if (_ray->isIntersectWith(veVec3::ZERO, veVec3(GIZMO_PLANE_SIZE, 0.0f, 0.0f) * _scale, veVec3(GIZMO_PLANE_SIZE, GIZMO_PLANE_SIZE, 0.0f) * _scale, nullptr, nullptr) ||
              _ray->isIntersectWith(veVec3::ZERO, veVec3(GIZMO_PLANE_SIZE, GIZMO_PLANE_SIZE, 0.0f) * _scale, veVec3(0.0f, GIZMO_PLANE_SIZE, 0.0f) * _scale, nullptr, nullptr)){
        drawAxes(veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 0.0f, 1.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(1.0f, 1.0f, 0.0f, 0.5f));
        _planeNormal = veVec3::UNIT_Z;
        at = AxesType::AT_XY;
    }else if (_ray->isIntersectWith(veVec3::ZERO, veVec3::UNIT_X * _scale, nullptr, 0.005f * _scale)){
        drawAxes(veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 0.0f, 1.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(0.0f, 0.0f, 1.0f, 0.2f));
        veVec3 right = _ray->getDirection().crossProduct(veVec3::UNIT_X);
        _planeNormal = right.crossProduct(veVec3::UNIT_X);
        at = AxesType::AT_X;
    }else if (_ray->isIntersectWith(veVec3::ZERO, veVec3::UNIT_Y * _scale, nullptr, 0.005f * _scale)){
        drawAxes(veVec4(1.0f, 0.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 0.0f, 1.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(0.0f, 0.0f, 1.0f, 0.2f));
        veVec3 right = _ray->getDirection().crossProduct(veVec3::UNIT_Y);
        _planeNormal = right.crossProduct(veVec3::UNIT_Y);
        at = AxesType::AT_Y;
    }else if (_ray->isIntersectWith(veVec3::ZERO, veVec3::UNIT_Z * _scale, nullptr, 0.005f * _scale)){
        drawAxes(veVec4(1.0f, 0.0f, 0.0f, 1.0f), veVec4(0.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(0.0f, 0.0f, 1.0f, 0.2f));
        veVec3 right = _ray->getDirection().crossProduct(veVec3::UNIT_Z);
        _planeNormal = right.crossProduct(veVec3::UNIT_Z);
        at = AxesType::AT_Z;
    }
    
    if (at != AxesType::AT_NONE){
        _planeNormal.normalize();
        _ray->isIntersectWith(vePlane(_planeNormal, 0.0f), &_latestPosition);
    }
    
    return at;
}

void veGizmoTranslation::touchMove(AxesType at, const veVec2 &screenCoords, veVec3 &pos, veVec3 &scl, veQuat &rot)
{
    veVec3 start = _sceneManager->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, -1.0f);
    veVec3 end = _sceneManager->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, 1.0f);
    veMat4 wton = _parents[0]->getWorldToNodeMatrix();
    _ray->setStart(wton * start);
    _ray->setEnd(wton * end);
    
    veVec3 currentPosition;
    _ray->isIntersectWith(vePlane(_planeNormal, 0.0f), &currentPosition);
    
    veVec3 deltaPos;
    switch (at) {
        case veGizmo::AxesType::AT_X:
        {
            deltaPos.x() = (currentPosition - _latestPosition).dotProduct(veVec3::UNIT_X);
        }
            break;
        case veGizmo::AxesType::AT_Y:
        {
            deltaPos.y() = (currentPosition - _latestPosition).dotProduct(veVec3::UNIT_Y);
        }
            break;
        case veGizmo::AxesType::AT_Z:
        {
            deltaPos.z() = (currentPosition - _latestPosition).dotProduct(veVec3::UNIT_Z);
        }
            break;
        case veGizmo::AxesType::AT_YZ:
        {
            deltaPos = currentPosition - _latestPosition;
        }
            break;
        case veGizmo::AxesType::AT_XZ:
        {
            deltaPos = currentPosition - _latestPosition;
        }
            break;
        case veGizmo::AxesType::AT_XY:
        {
            deltaPos = currentPosition - _latestPosition;
        }
            break;
        default:
            break;
    }

    pos = deltaPos;
}

void veGizmoTranslation::touchUp()
{
    drawAxes(veVec4(1.0f, 0.0f, 0.0f, 1.0f), veVec4(0.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 0.0f, 1.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(0.0f, 0.0f, 1.0f, 0.2f));
}

bool veGizmoTranslation::intersectWith(veRay *ray, veNode *node)
{
    return false;
}

void veGizmoTranslation::drawAxes(const veVec4 &axesXCol, const veVec4 &axesYCol, const veVec4 &axesZCol, const veVec4 &YZCol, const veVec4 &XZCol, const veVec4 &XYCol)
{
    auto gizmoRenderer = static_cast<veGizmoRenderer *>(_renderer.get());
    gizmoRenderer->refresh();
    gizmoRenderer->drawLines(veVec3::ZERO, veVec3::UNIT_X, axesXCol);
    gizmoRenderer->drawCone(veVec3::UNIT_X - veVec3(0.2f, 0.0f, 0.0f), veVec3::UNIT_X, 0.05f, 0.2f, axesXCol);
    gizmoRenderer->drawLines(veVec3::ZERO, veVec3::UNIT_Y, axesYCol);
    gizmoRenderer->drawCone(veVec3::UNIT_Y - veVec3(0.0f, 0.2f, 0.0f), veVec3::UNIT_Y, 0.05f, 0.2f, axesYCol);
    gizmoRenderer->drawLines(veVec3::ZERO, veVec3::UNIT_Z, axesZCol);
    gizmoRenderer->drawCone(veVec3::UNIT_Z - veVec3(0.0f, 0.0f, 0.2f), veVec3::UNIT_Z, 0.05f, 0.2f, axesZCol);
    
    gizmoRenderer->drawQuat(veVec3(0.0f, 0.0f, GIZMO_PLANE_SIZE), veVec3::ZERO, veVec3(0.0f, GIZMO_PLANE_SIZE, 0.0f), veVec3(0.0f, GIZMO_PLANE_SIZE, GIZMO_PLANE_SIZE), YZCol);
    gizmoRenderer->drawLines(veVec3(0.0f, 0.0f, GIZMO_PLANE_SIZE), veVec3::ZERO, veVec4(YZCol.x(), YZCol.y(), YZCol.z(), 1.0f));
    gizmoRenderer->drawLines(veVec3::ZERO, veVec3(0.0f, GIZMO_PLANE_SIZE, 0.0f), veVec4(YZCol.x(), YZCol.y(), YZCol.z(), 1.0f));
    gizmoRenderer->drawLines(veVec3(0.0f, GIZMO_PLANE_SIZE, 0.0f), veVec3(0.0f, GIZMO_PLANE_SIZE, GIZMO_PLANE_SIZE), veVec4(YZCol.x(), YZCol.y(), YZCol.z(), 1.0f));
    gizmoRenderer->drawLines(veVec3(0.0f, GIZMO_PLANE_SIZE, GIZMO_PLANE_SIZE), veVec3(0.0f, 0.0f, GIZMO_PLANE_SIZE), veVec4(YZCol.x(), YZCol.y(), YZCol.z(), 1.0f));
    
    gizmoRenderer->drawQuat(veVec3::ZERO, veVec3(0.0f, 0.0f, GIZMO_PLANE_SIZE), veVec3(GIZMO_PLANE_SIZE, 0.0f, GIZMO_PLANE_SIZE), veVec3(GIZMO_PLANE_SIZE, 0.0f, 0.0f), XZCol);
    gizmoRenderer->drawLines(veVec3::ZERO, veVec3(0.0f, 0.0f, GIZMO_PLANE_SIZE), veVec4(XZCol.x(), XZCol.y(), XZCol.z(), 1.0f));
    gizmoRenderer->drawLines(veVec3(0.0f, 0.0f, GIZMO_PLANE_SIZE), veVec3(GIZMO_PLANE_SIZE, 0.0f, GIZMO_PLANE_SIZE), veVec4(XZCol.x(), XZCol.y(), XZCol.z(), 1.0f));
    gizmoRenderer->drawLines(veVec3(GIZMO_PLANE_SIZE, 0.0f, GIZMO_PLANE_SIZE), veVec3(GIZMO_PLANE_SIZE, 0.0f, 0.0f), veVec4(XZCol.x(), XZCol.y(), XZCol.z(), 1.0f));
    gizmoRenderer->drawLines(veVec3(GIZMO_PLANE_SIZE, 0.0f, 0.0f), veVec3::ZERO, veVec4(XZCol.x(), XZCol.y(), XZCol.z(), 1.0f));
    
    gizmoRenderer->drawQuat(veVec3::ZERO, veVec3(GIZMO_PLANE_SIZE, 0.0f, 0.0f), veVec3(GIZMO_PLANE_SIZE, GIZMO_PLANE_SIZE, 0.0f), veVec3(0.0f, GIZMO_PLANE_SIZE, 0.0f), XYCol);
    gizmoRenderer->drawLines(veVec3::ZERO, veVec3(GIZMO_PLANE_SIZE, 0.0f, 0.0f), veVec4(XYCol.x(), XYCol.y(), XYCol.z(), 1.0f));
    gizmoRenderer->drawLines(veVec3(GIZMO_PLANE_SIZE, 0.0f, 0.0f), veVec3(GIZMO_PLANE_SIZE, GIZMO_PLANE_SIZE, 0.0f), veVec4(XYCol.x(), XYCol.y(), XYCol.z(), 1.0f));
    gizmoRenderer->drawLines(veVec3(GIZMO_PLANE_SIZE, GIZMO_PLANE_SIZE, 0.0f), veVec3(0.0f, GIZMO_PLANE_SIZE, 0.0f), veVec4(XYCol.x(), XYCol.y(), XYCol.z(), 1.0f));
    gizmoRenderer->drawLines(veVec3(0.0f, GIZMO_PLANE_SIZE, 0.0f), veVec3::ZERO, veVec4(XYCol.x(), XYCol.y(), XYCol.z(), 1.0f));
}
