#include "GizmoComponent.h"
#include "KernelCore/SceneManager.h"
#include "KernelCore/Viewer.h"

#define GIZMO_SCALE_SQUARED_DISTANCE 10.0f

veGizmoComponent::veGizmoComponent()
    : _type(GizmoType::GT_TRANSLATION)
    , _gizmoAxesType(veGizmo::AxesType::AT_NONE)
    , _refresh(true)
{

}

veGizmoComponent::~veGizmoComponent()
{

}

bool veGizmoComponent::handle(veSceneManager *sm, veViewer *viewer, const veEvent &event)
{
    if (!_gizmo.valid()) return false;
    
    switch (event.getEventType()) {
        case veEvent::VE_PRESS:
        case veEvent::VE_TOUCH_START:
        {
            veVec2 screenCoords;
            if (event.getEventType() == veEvent::VE_PRESS){
                screenCoords = veVec2(event.getMouseX(), event.getMouseY());
            }else{
                auto touch = event.getTouches()[0];
                screenCoords = veVec2(touch.x, touch.y);
            }
            
            veVec3 ntowPos, camtowPos;
            _attachedNode->getMatrix().decomposition(&ntowPos, nullptr, nullptr);
            viewer->getCamera()->getNodeToWorldMatrix().decomposition(&camtowPos, nullptr, nullptr);
            veReal dis = (camtowPos - ntowPos).length();
            //_gizmoNode->setMatrix(veMat4::transform(ntowPos, veVec3(dis / GIZMO_SCALE_SQUARED_DISTANCE), nodeRot));
            veReal scl = _gizmo->getScale();
            
            _gizmo->setScale(dis / GIZMO_SCALE_SQUARED_DISTANCE);
            _gizmoAxesType = _gizmo->touchDown(viewer, screenCoords);
            _gizmo->setScale(scl);
            
            if (_gizmoAxesType != veGizmo::AxesType::AT_NONE)
                return true;
        }
            break;
        case veEvent::VE_DRAG:
        case veEvent::VE_TOUCH_MOVE:
        {
            veVec2 screenCoords;
            if (event.getEventType() == veEvent::VE_DRAG){
                screenCoords = veVec2(event.getMouseX(), event.getMouseY());
            }else{
                auto touch = event.getTouches()[0];
                screenCoords = veVec2(touch.x, touch.y);
            }
            if (_gizmoAxesType != veGizmo::AxesType::AT_NONE){
                veVec3 pos,scl; veQuat rot;
                _gizmo->touchMove(viewer, _gizmoAxesType, screenCoords, pos, scl, rot);
                applyGizmoMatrix(pos, scl, rot);
                return true;
            }
        }
            break;
        case veEvent::VE_RELEASE:
        case veEvent::VE_TOUCH_END:
        {
            _gizmo->touchUp();
            _gizmoAxesType = veGizmo::AxesType::AT_NONE;
        }
            break;
            
        default:
            break;
    }
    
    return false;
}

void veGizmoComponent::update(veSceneManager *sm)
{
    if (_refresh){
        _attachedNode->removeRenderableObject(_gizmo.get());
        if (_type == GizmoType::GT_TRANSLATION){
            _gizmo = new veGizmoTranslation(sm);
        } else if (_type == GizmoType::GT_ROTATION){
            _gizmo = new veGizmoRotation(sm);
        }else {
            _gizmo = new veGizmoScale(sm);
        }
        _attachedNode->addRenderableObject(_gizmo.get());
        caculateGizmoMatrix();
        _refresh = false;
    }
}

void veGizmoComponent::beforeRender(veSceneManager *sm, veViewer *viewer)
{
    veVec3 ntowPos, camtowPos;
    _attachedNode->getMatrix().decomposition(&ntowPos, nullptr, nullptr);
    viewer->getCamera()->getNodeToWorldMatrix().decomposition(&camtowPos, nullptr, nullptr);
    veReal dis = (camtowPos - ntowPos).length();
    //_gizmoNode->setMatrix(veMat4::transform(ntowPos, veVec3(dis / GIZMO_SCALE_SQUARED_DISTANCE), nodeRot));
    _gizmo->setScale(dis / GIZMO_SCALE_SQUARED_DISTANCE);
}

void veGizmoComponent::caculateGizmoMatrix()
{
    if (_handleNodeList.empty()) return;

    veVec3 position, scale; veQuat rotate;
    veMat4 nTow;
    for (auto &handleNode : _handleNodeList){
        nTow = handleNode->getNodeToWorldMatrix();
        veVec3 nodePos;
        nTow.decomposition(&nodePos, &scale, &rotate);
        position += nodePos;
    }
    position /= _handleNodeList.size();
    updateGizmo(veMat4::transform(position, veVec3::UNIT_SCALE, rotate));
}

void veGizmoComponent::applyGizmoMatrix(const veVec3 &trans, const veVec3 &scl, const veQuat &rot)
{
    if (_handleNodeList.empty()) return;
    
    static veVec3 totalGizmoScale = veVec3::UNIT_SCALE;
    veVec3 npos, nscl; veQuat nrot;
    veMat4 gizmoMatInv = _attachedNode->getMatrix();
    gizmoMatInv.inverse();
    for (auto &handleNode : _handleNodeList) {
        veMat4 nodeIngizmoMat = gizmoMatInv * handleNode->getNodeToWorldMatrix();
        nodeIngizmoMat.decomposition(&npos, &nscl, &nrot);
        nodeIngizmoMat = _attachedNode->getMatrix() * veMat4::transform(trans, scl + veVec3::UNIT_SCALE, rot) * nodeIngizmoMat;
        veMat4 toParentMat = handleNode->getParent()? handleNode->getParent()->getWorldToNodeMatrix() : veMat4::IDENTITY;
        handleNode->setMatrix(toParentMat * nodeIngizmoMat);
    }
    
    totalGizmoScale = scl;
    updateGizmo(_attachedNode->getMatrix() * veMat4::transform(trans, veVec3::UNIT_SCALE, rot));
}

void veGizmoComponent::updateGizmo(const veMat4 &mat)
{
    veVec3 npos; veQuat nrot;
    mat.decomposition(&npos, nullptr, &nrot);
    _attachedNode->setMatrix(veMat4::transform(npos, veVec3::UNIT_SCALE, nrot));
}

void veGizmoComponent::setGizmoType(GizmoType type)
{
    if (_type == type) return;
    _type = type;
    _refresh = true;
}

void veGizmoComponent::addHandleNode(veNode *node)
{
    auto iter = std::find(_handleNodeList.begin(), _handleNodeList.end(), node);
    if (iter != _handleNodeList.end()) return;
    _handleNodeList.push_back(node);
    _refresh = true;
}

void veGizmoComponent::removeHandleNode(veNode *node)
{
    auto iter = std::find(_handleNodeList.begin(), _handleNodeList.end(), node);
    if (iter == _handleNodeList.end()) return;
    _handleNodeList.erase(iter);
    _refresh = true;
}
