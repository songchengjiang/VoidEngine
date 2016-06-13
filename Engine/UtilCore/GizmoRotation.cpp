#include "GizmoRotation.h"
#include "KernelCore/SceneManager.h"
#include "GizmoRenderer.h"

#define GIZMO_CIRCLE_RADIUS 0.5f
#define GIZMO_CIRCLE_SIZE (GIZMO_CIRCLE_RADIUS * 2.0f)

veGizmoRotation::veGizmoRotation(veSceneManager *sm)
    : veGizmo(sm)
{
    drawAxes(veVec4(1.0f, 0.0f, 0.0f, 1.0f), veVec4(0.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 0.0f, 1.0f, 1.0f));
}

veGizmoRotation::~veGizmoRotation()
{

}

veGizmoRotation::AxesType veGizmoRotation::touchDown(const veVec2 &screenCoords)
{
    veVec3 start = _sceneManager->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, -1.0f);
    veVec3 end = _sceneManager->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, 1.0f);
    veMat4 wton = _parents[0]->getWorldToNodeMatrix();
    _ray->setStart(wton * start);
    _ray->setEnd(wton * end);
    
    AxesType at = AxesType::AT_NONE;
    do {
        veVec3 intersectPoint;
        if (_ray->isIntersectWith(veVec3(0.0f, -GIZMO_CIRCLE_SIZE, GIZMO_CIRCLE_SIZE) * _scale, veVec3(0.0f, -GIZMO_CIRCLE_SIZE, -GIZMO_CIRCLE_SIZE) * _scale, veVec3(0.0f, GIZMO_CIRCLE_SIZE, -GIZMO_CIRCLE_SIZE) * _scale, &intersectPoint, nullptr) ||
            _ray->isIntersectWith(veVec3(0.0f, -GIZMO_CIRCLE_SIZE, GIZMO_CIRCLE_SIZE) * _scale, veVec3(0.0f, GIZMO_CIRCLE_SIZE, -GIZMO_CIRCLE_SIZE) * _scale, veVec3(0.0f, GIZMO_CIRCLE_SIZE, GIZMO_CIRCLE_SIZE) * _scale, &intersectPoint, nullptr)){
            if (veMath::veAbs(intersectPoint.length() - GIZMO_CIRCLE_RADIUS * _scale) < 0.05 * _scale){
                drawAxes(veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 0.0f, 1.0f, 1.0f));
                _planeNormal = veVec3::UNIT_X;
                at = AxesType::AT_YZ;
                break;
            }
        }
        
        if (_ray->isIntersectWith(veVec3(-GIZMO_CIRCLE_SIZE, 0.0f, -GIZMO_CIRCLE_SIZE) * _scale, veVec3(-GIZMO_CIRCLE_SIZE, 0.0f, GIZMO_CIRCLE_SIZE) * _scale, veVec3(GIZMO_CIRCLE_SIZE, 0.0f, GIZMO_CIRCLE_SIZE) * _scale, &intersectPoint, nullptr) ||
            _ray->isIntersectWith(veVec3(-GIZMO_CIRCLE_SIZE, 0.0f, -GIZMO_CIRCLE_SIZE) * _scale, veVec3(GIZMO_CIRCLE_SIZE, 0.0f, GIZMO_CIRCLE_SIZE) * _scale, veVec3(GIZMO_CIRCLE_SIZE, 0.0f, -GIZMO_CIRCLE_SIZE) * _scale, &intersectPoint, nullptr)){
            if (veMath::veAbs(intersectPoint.length() - GIZMO_CIRCLE_RADIUS * _scale) < 0.05 * _scale){
                drawAxes(veVec4(1.0f, 0.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 0.0f, 1.0f, 1.0f));
                _planeNormal = veVec3::UNIT_Y;
                at = AxesType::AT_XZ;
                break;
            }
        }
        
        if (_ray->isIntersectWith(veVec3(-GIZMO_CIRCLE_SIZE, -GIZMO_CIRCLE_SIZE, 0.0f) * _scale, veVec3(GIZMO_CIRCLE_SIZE, -GIZMO_CIRCLE_SIZE, 0.0f) * _scale, veVec3(GIZMO_CIRCLE_SIZE, GIZMO_CIRCLE_SIZE, 0.0f) * _scale, &intersectPoint, nullptr) ||
            _ray->isIntersectWith(veVec3(-GIZMO_CIRCLE_SIZE, -GIZMO_CIRCLE_SIZE, 0.0f) * _scale, veVec3(GIZMO_CIRCLE_SIZE, GIZMO_CIRCLE_SIZE, 0.0f) * _scale, veVec3(-GIZMO_CIRCLE_SIZE, GIZMO_CIRCLE_SIZE, 0.0f) * _scale, &intersectPoint, nullptr)){
            if (veMath::veAbs(intersectPoint.length() - GIZMO_CIRCLE_RADIUS * _scale) < 0.05 * _scale){
                drawAxes(veVec4(1.0f, 0.0f, 0.0f, 1.0f), veVec4(0.0f, 1.0f, 0.0f, 1.0f), veVec4(1.0f, 1.0f, 0.0f, 1.0f));
                _planeNormal = veVec3::UNIT_Z;
                at = AxesType::AT_XY;
                break;
            }
        }
    } while (false);
    
    if (at != AxesType::AT_NONE){
        _ray->isIntersectWith(vePlane(_planeNormal, 0.0f), &_latestPosition);
    }
    
    return at;
}

void veGizmoRotation::touchMove(AxesType at, const veVec2 &screenCoords, veVec3 &pos, veVec3 &scl, veQuat &rot)
{
    veVec3 start = _sceneManager->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, -1.0f);
    veVec3 end = _sceneManager->getCamera()->convertScreenCoordsToWorldCoords(screenCoords, 1.0f);
    veMat4 wton = _parents[0]->getWorldToNodeMatrix();
    _ray->setStart(wton * start);
    _ray->setEnd(wton * end);
    
    veVec3 currentPosition;
    _ray->isIntersectWith(vePlane(_planeNormal, 0.0f), &currentPosition);
    veVec3 latestPosition = _latestPosition;
    currentPosition.normalize();
    latestPosition.normalize();
    veVec3 dir = latestPosition.crossProduct(currentPosition);
    veReal angle = veMath::veAcos(currentPosition.dotProduct(latestPosition));
    dir.normalize();
    rot = veQuat(angle, dir);
}

void veGizmoRotation::touchUp()
{
    drawAxes(veVec4(1.0f, 0.0f, 0.0f, 1.0f), veVec4(0.0f, 1.0f, 0.0f, 1.0f), veVec4(0.0f, 0.0f, 1.0f, 1.0f));
}

bool veGizmoRotation::intersectWith(veRay *ray, veNode *node)
{
    return false;
}

void veGizmoRotation::drawAxes(const veVec4 &YZCol, const veVec4 &XZCol, const veVec4 &XYCol)
{
    auto gizmoRenderer = static_cast<veGizmoRenderer *>(_renderer.get());
    gizmoRenderer->refresh();
    gizmoRenderer->drawCircle(veVec3::UNIT_X, GIZMO_CIRCLE_RADIUS, YZCol);
    gizmoRenderer->drawCircle(veVec3::UNIT_Y, GIZMO_CIRCLE_RADIUS, XZCol);
    gizmoRenderer->drawCircle(veVec3::UNIT_Z, GIZMO_CIRCLE_RADIUS, XYCol);
}
