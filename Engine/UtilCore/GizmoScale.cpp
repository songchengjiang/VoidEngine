#include "GizmoScale.h"
#include "KernelCore/SceneManager.h"
#include "GizmoRenderer.h"
#include "KernelCore/Viewer.h"

#define GIZMO_PLANE_SIZE 0.25f

veGizmoScale::veGizmoScale(veSceneManager *sm)
    : veGizmo(sm)
    , _xLength(0.0f)
    , _yLength(0.0f)
    , _zLength(0.0f)
{
    drawAxes(veVec4(1.0f, 0.0f, 0.0f, 1.0f), veVec4(0.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 0.0f, 1.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(0.0f, 0.0f, 1.0f, 0.2f), veVec4::WHITE);
}

veGizmoScale::~veGizmoScale()
{

}

bool veGizmoScale::intersectWith(veRay *ray, veNode *node)
{
    return false;
}

veGizmoScale::AxesType veGizmoScale::touchDown(veViewer *viewer, const veVec2 &screenCoords)
{
    veVec3 start = viewer->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, -1.0f);
    veVec3 end = viewer->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, 1.0f);
    veMat4 wton = _parents[0]->getWorldToNodeMatrix();
    _ray->setStart(wton * start);
    _ray->setEnd(wton * end);
    
    AxesType at = AxesType::AT_NONE;
    if (_ray->isIntersectWith(veBoundingBox(veVec3(-0.05 * _scale), veVec3(0.05f * _scale)))) {
        drawAxes(veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 0.5f), veVec4(1.0f, 1.0f, 0.0f, 0.5f), veVec4(1.0f, 1.0f, 0.0f, 0.5f), veVec4(1.0f, 1.0f, 0.0f, 1.0f));
        _planeNormal = veVec3::UNIT_SCALE;
        at = AxesType::AT_XYZ;
    }else if (_ray->isIntersectWith(veVec3(0.0f, 0.0f, GIZMO_PLANE_SIZE) * _scale, veVec3::ZERO, veVec3(0.0f, GIZMO_PLANE_SIZE, 0.0f) * _scale, nullptr, nullptr)){
        drawAxes(veVec4(1.0f, 0.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 0.5f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(0.0f, 0.0f, 1.0f, 0.2f), veVec4::WHITE);
        _planeNormal = veVec3::UNIT_X;
        at = AxesType::AT_YZ;
    }else if (_ray->isIntersectWith(veVec3::ZERO, veVec3(0.0f, 0.0f, GIZMO_PLANE_SIZE) * _scale, veVec3(GIZMO_PLANE_SIZE, 0.0f, 0.0f) * _scale, nullptr, nullptr)){
        drawAxes(veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(1.0f, 1.0f, 0.0f, 0.5f), veVec4(0.0f, 0.0f, 1.0f, 0.2f), veVec4::WHITE);
        _planeNormal = veVec3::UNIT_Y;
        at = AxesType::AT_XZ;
    }else if (_ray->isIntersectWith(veVec3::ZERO, veVec3(GIZMO_PLANE_SIZE, 0.0f, 0.0f) * _scale, veVec3(0.0f, GIZMO_PLANE_SIZE, 0.0f) * _scale, nullptr, nullptr)){
        drawAxes(veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 0.0f, 1.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(1.0f, 1.0f, 0.0f, 0.5f), veVec4::WHITE);
        _planeNormal = veVec3::UNIT_Z;
        at = AxesType::AT_XY;
    }else if (_ray->isIntersectWith(veVec3::ZERO, veVec3::UNIT_X * _scale, nullptr, 0.005f * _scale)){
        drawAxes(veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 0.0f, 1.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(0.0f, 0.0f, 1.0f, 0.2f), veVec4::WHITE);
        veVec3 right = _ray->getDirection().crossProduct(veVec3::UNIT_X);
        _planeNormal = right.crossProduct(veVec3::UNIT_X);
        at = AxesType::AT_X;
    }else if (_ray->isIntersectWith(veVec3::ZERO, veVec3::UNIT_Y * _scale, nullptr, 0.005f * _scale)){
        drawAxes(veVec4(1.0f, 0.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 0.0f, 1.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(0.0f, 0.0f, 1.0f, 0.2f), veVec4::WHITE);
        veVec3 right = _ray->getDirection().crossProduct(veVec3::UNIT_Y);
        _planeNormal = right.crossProduct(veVec3::UNIT_Y);
        at = AxesType::AT_Y;
    }else if (_ray->isIntersectWith(veVec3::ZERO, veVec3::UNIT_Z * _scale, nullptr, 0.005f * _scale)){
        drawAxes(veVec4(1.0f, 0.0f, 0.0f, 1.0f), veVec4(0.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(0.0f, 0.0f, 1.0f, 0.2f), veVec4::WHITE);
        veVec3 right = _ray->getDirection().crossProduct(veVec3::UNIT_Z);
        _planeNormal = right.crossProduct(veVec3::UNIT_Z);
        at = AxesType::AT_Z;
    }
    
    if (at != AxesType::AT_NONE){
        _planeNormal.normalize();
        _ray->isIntersectWith(vePlane(_planeNormal, 0.0f), &_latestPosition);
        _xLength = _yLength = _zLength = 0.0f;
    }
    
    _latestscreenCoords = screenCoords;
    return at;
}

void veGizmoScale::touchMove(veViewer *viewer, AxesType at, const veVec2 &screenCoords, veVec3 &pos, veVec3 &scl, veQuat &rot)
{
    veVec3 start = viewer->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, -1.0f);
    veVec3 end = viewer->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, 1.0f);
    veMat4 wton = _parents[0]->getWorldToNodeMatrix();
    _ray->setStart(wton * start);
    _ray->setEnd(wton * end);
    
    veVec3 currentPosition;
    _ray->isIntersectWith(vePlane(_planeNormal, 0.0f), &currentPosition);
    
    veVec3 deltaScl;
    switch (at) {
        case veGizmo::AxesType::AT_X:
        {
            deltaScl.x() = (currentPosition - _latestPosition).dotProduct(veVec3::UNIT_X);
            _xLength += deltaScl.x();
            drawAxes(veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 0.0f, 1.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(0.0f, 0.0f, 1.0f, 0.2f), veVec4::WHITE, 1.0f + _xLength / _scale);
        }
            break;
        case veGizmo::AxesType::AT_Y:
        {
            deltaScl.y() = (currentPosition - _latestPosition).dotProduct(veVec3::UNIT_Y);
            _yLength += deltaScl.y();
            drawAxes(veVec4(1.0f, 0.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 0.0f, 1.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(0.0f, 0.0f, 1.0f, 0.2f), veVec4::WHITE, 1.0f, 1.0f + _yLength / _scale);
        }
            break;
        case veGizmo::AxesType::AT_Z:
        {
            deltaScl.z() = (currentPosition - _latestPosition).dotProduct(veVec3::UNIT_Z);
            _zLength += deltaScl.z();
            drawAxes(veVec4(1.0f, 0.0f, 0.0f, 1.0f), veVec4(0.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(0.0f, 0.0f, 1.0f, 0.2f), veVec4::WHITE, 1.0f, 1.0f, 1.0f + _zLength / _scale);
        }
            break;
        case veGizmo::AxesType::AT_YZ:
        {
            deltaScl.y() = (currentPosition - _latestPosition).dotProduct(veVec3::UNIT_Y);
            deltaScl.z() = (currentPosition - _latestPosition).dotProduct(veVec3::UNIT_Z);
            veReal val = veMath::veAbs(deltaScl.y()) < veMath::veAbs(deltaScl.z())? deltaScl.z(): deltaScl.y();
            deltaScl.z() = deltaScl.y() = val;
            _yLength += deltaScl.y();
            _zLength += deltaScl.z();
            drawAxes(veVec4(1.0f, 0.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 0.5f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(0.0f, 0.0f, 1.0f, 0.2f), veVec4::WHITE, 1.0f, 1.0f + _yLength / _scale, 1.0f + _zLength / _scale);
        }
            break;
        case veGizmo::AxesType::AT_XZ:
        {
            deltaScl.x() = (currentPosition - _latestPosition).dotProduct(veVec3::UNIT_X);
            deltaScl.z() = (currentPosition - _latestPosition).dotProduct(veVec3::UNIT_Z);
            veReal val = veMath::veAbs(deltaScl.x()) < veMath::veAbs(deltaScl.z())? deltaScl.z(): deltaScl.x();
            deltaScl.z() = deltaScl.x() = val;
            _xLength += deltaScl.x();
            _zLength += deltaScl.z();
            drawAxes(veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(1.0f, 1.0f, 0.0f, 0.5f), veVec4(0.0f, 0.0f, 1.0f, 0.2f), veVec4::WHITE, 1.0f + _xLength / _scale, 1.0f, 1.0f + _zLength / _scale);
        }
            break;
        case veGizmo::AxesType::AT_XY:
        {
            deltaScl.x() = (currentPosition - _latestPosition).dotProduct(veVec3::UNIT_X);
            deltaScl.y() = (currentPosition - _latestPosition).dotProduct(veVec3::UNIT_Y);
            veReal val = veMath::veAbs(deltaScl.x()) < veMath::veAbs(deltaScl.y())? deltaScl.y(): deltaScl.x();
            deltaScl.y() = deltaScl.x() = val;
            _xLength += deltaScl.x();
            _yLength += deltaScl.y();
            drawAxes(veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 0.0f, 1.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(1.0f, 1.0f, 0.0f, 0.5f), veVec4::WHITE, 1.0f + _xLength / _scale, 1.0f + _yLength / _scale, 1.0f);
        }
            break;
        case veGizmo::AxesType::AT_XYZ:
        {
            deltaScl.x() = (screenCoords - _latestscreenCoords).dotProduct(veVec2::UNIT_X);
            deltaScl.y() = (screenCoords - _latestscreenCoords).dotProduct(veVec2::UNIT_Y);
//            deltaScl.z() = (currentPosition - _latestPosition).dotProduct(veVec3::UNIT_Z);
            veReal val = veMath::veAbs(deltaScl.x()) < veMath::veAbs(deltaScl.y())? deltaScl.y(): deltaScl.x();
//            val = veMath::veAbs(val) < veMath::veAbs(deltaScl.z())? deltaScl.z(): val;
            //veReal val = deltaScl.x();
            veReal sign = 0.0f < val? 1.0f: -1.0f;
            deltaScl.z() = deltaScl.y() = deltaScl.x() = sign * (currentPosition - _latestPosition).length();
            _xLength += deltaScl.x();
            _yLength += deltaScl.y();
            _zLength += deltaScl.z();
            drawAxes(veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 0.5f), veVec4(1.0f, 1.0f, 0.0f, 0.5f), veVec4(1.0f, 1.0f, 0.0f, 0.5f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), 1.0f + _xLength / _scale, 1.0f + _yLength / _scale, 1.0f + _zLength / _scale);
        }
            break;
        default:
            break;
    }
    
    _latestPosition = currentPosition;
    _latestscreenCoords = screenCoords;
    
    scl = deltaScl * 0.1f;
    veLog("(%f, %f, %f)\n", scl.x(), scl.y(), scl.z());
}

void veGizmoScale::touchUp()
{
    drawAxes(veVec4(1.0f, 0.0f, 0.0f, 1.0f), veVec4(0.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 0.0f, 1.0f, 1.0f), veVec4(1.0f, 0.0f, 0.0f, 0.2f), veVec4(0.0f, 1.0f, 0.0f, 0.2f), veVec4(0.0f, 0.0f, 1.0f, 0.2f), veVec4::WHITE);
}

void veGizmoScale::drawAxes(const veVec4 &axesXCol, const veVec4 &axesYCol, const veVec4 &axesZCol
                          , const veVec4 &YZCol, const veVec4 &XZCol, const veVec4 &XYCol, const veVec4 &XYZCol
                          , veReal xLength, veReal yLength, veReal zLength)
{
    auto gizmoRenderer = static_cast<veGizmoRenderer *>(_renderer.get());
    gizmoRenderer->refresh();
    gizmoRenderer->drawLines(veVec3::ZERO, veVec3(xLength, 0.0f, 0.0f), axesXCol);
    gizmoRenderer->drawBox(veVec3(xLength, 0.0f, 0.0f) - veVec3(0.05f, 0.0f, 0.0f), 0.05f, axesXCol);
    gizmoRenderer->drawLines(veVec3::ZERO, veVec3(0.0f, yLength, 0.0f), axesYCol);
    gizmoRenderer->drawBox(veVec3(0.0f, yLength, 0.0f) - veVec3(0.0f, 0.05f, 0.0f), 0.05f, axesYCol);
    gizmoRenderer->drawLines(veVec3::ZERO, veVec3(0.0f, 0.0f, zLength), axesZCol);
    gizmoRenderer->drawBox(veVec3(0.0f, 0.0f, zLength) - veVec3(0.0f, 0.0f, 0.05f), 0.05f, axesZCol);
    
    gizmoRenderer->drawTri(veVec3(0.0f, 0.0f, GIZMO_PLANE_SIZE), veVec3::ZERO, veVec3(0.0f, GIZMO_PLANE_SIZE, 0.0f), YZCol);
    gizmoRenderer->drawLines(veVec3(0.0f, 0.0f, GIZMO_PLANE_SIZE), veVec3::ZERO, veVec4(YZCol.x(), YZCol.y(), YZCol.z(), 1.0f));
    gizmoRenderer->drawLines(veVec3::ZERO, veVec3(0.0f, GIZMO_PLANE_SIZE, 0.0f), veVec4(YZCol.x(), YZCol.y(), YZCol.z(), 1.0f));
    gizmoRenderer->drawLines(veVec3(0.0f, GIZMO_PLANE_SIZE, 0.0f), veVec3(0.0f, 0.0f, GIZMO_PLANE_SIZE), veVec4(YZCol.x(), YZCol.y(), YZCol.z(), 1.0f));
    
    gizmoRenderer->drawTri(veVec3::ZERO, veVec3(0.0f, 0.0f, GIZMO_PLANE_SIZE), veVec3(GIZMO_PLANE_SIZE, 0.0f, 0.0f), XZCol);
    gizmoRenderer->drawLines(veVec3::ZERO, veVec3(0.0f, 0.0f, GIZMO_PLANE_SIZE), veVec4(XZCol.x(), XZCol.y(), XZCol.z(), 1.0f));
    gizmoRenderer->drawLines(veVec3(0.0f, 0.0f, GIZMO_PLANE_SIZE), veVec3(GIZMO_PLANE_SIZE, 0.0f, 0.0f), veVec4(XZCol.x(), XZCol.y(), XZCol.z(), 1.0f));
    gizmoRenderer->drawLines(veVec3(GIZMO_PLANE_SIZE, 0.0f, 0.0f), veVec3::ZERO, veVec4(XZCol.x(), XZCol.y(), XZCol.z(), 1.0f));
    
    
    gizmoRenderer->drawTri(veVec3::ZERO, veVec3(GIZMO_PLANE_SIZE, 0.0f, 0.0f), veVec3(0.0f, GIZMO_PLANE_SIZE, 0.0f), XYCol);
    gizmoRenderer->drawLines(veVec3::ZERO, veVec3(GIZMO_PLANE_SIZE, 0.0f, 0.0f), veVec4(XYCol.x(), XYCol.y(), XYCol.z(), 1.0f));
    gizmoRenderer->drawLines(veVec3(GIZMO_PLANE_SIZE, 0.0f, 0.0f), veVec3(0.0f, GIZMO_PLANE_SIZE, 0.0f), veVec4(XYCol.x(), XYCol.y(), XYCol.z(), 1.0f));
    gizmoRenderer->drawLines(veVec3(0.0f, GIZMO_PLANE_SIZE, 0.0f), veVec3::ZERO, veVec4(XYCol.x(), XYCol.y(), XYCol.z(), 1.0f));
    
    gizmoRenderer->drawBox(veVec3::ZERO, 0.05f, XYZCol);
}

