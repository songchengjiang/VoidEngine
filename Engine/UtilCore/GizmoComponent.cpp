#include "GizmoComponent.h"
#include "KernelCore/SceneManager.h"

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

bool veGizmoComponent::handle(veSceneManager *sm, const veEvent &event)
{
    if (!_gizmo.valid()) return false;
    
    switch (event.getEventType()) {
        case veEvent::VE_PRESS:
        {
            _gizmoAxesType = _gizmo->touchDown(veVec2(event.getMouseX(), event.getMouseY()));
            if (_gizmoAxesType != veGizmo::AxesType::AT_NONE)
                return true;
        }
            break;
        case veEvent::VE_DRAG:
        {
            if (_gizmoAxesType != veGizmo::AxesType::AT_NONE){
                veVec3 pos,scl; veQuat rot;
                _gizmo->touchMove(_gizmoAxesType, veVec2(event.getMouseX(), event.getMouseY()), pos, scl, rot);
                applyGizmoMatrix(pos, scl, rot);
                return true;
            }
        }
            break;
        case veEvent::VE_RELEASE:
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

void veGizmoComponent::afterUpdate(veSceneManager *sm)
{
    if (_refresh){
        _gizmoNode->removeRenderableObject(_gizmo.get());
        if (_type == GizmoType::GT_TRANSLATION){
            _gizmo = new veGizmoTranslation(sm);
        } else if (_type == GizmoType::GT_ROTATION){
            _gizmo = new veGizmoRotation(sm);
        }else {
            _gizmo = new veGizmoScale(sm);
        }
        _gizmoNode->addRenderableObject(_gizmo.get());
        caculateGizmoMatrix();
        _refresh = false;
    }
    
    veVec3 ntowPos, camtowPos;
    veQuat nodeRot;
    _gizmoNode->getMatrix().decomposition(&ntowPos, nullptr, &nodeRot);
    sm->getCamera()->getNodeToWorldMatrix().decomposition(&camtowPos, nullptr, nullptr);
    veReal dis = (camtowPos - ntowPos).length();
    //_gizmoNode->setMatrix(veMat4::transform(ntowPos, veVec3(dis / GIZMO_SCALE_SQUARED_DISTANCE), nodeRot));
    _gizmo->setScale(dis / GIZMO_SCALE_SQUARED_DISTANCE);
}

bool veGizmoComponent::onAttachToNode(veNode *node)
{
    bool state = veComponent::onAttachToNode(node);
    if (state){
        if (_attachedNodeList.size() <= 1){
            if (!_gizmoNode.valid())
                _gizmoNode = node->getSceneManager()->createNode("_VE_GIZMO_NODE_");
            node->getSceneManager()->getRootNode()->addChild(_gizmoNode.get());
        }
        _refresh = true;
    }
    return state;
}

bool veGizmoComponent::onDetachToNode(veNode *node)
{
    bool state = veComponent::onDetachToNode(node);
    if (state) {
        if (_attachedNodeList.empty()){
            node->getSceneManager()->getRootNode()->removeChild(_gizmoNode.get());
        }
        _refresh = true;
    }

    return state;
}

void veGizmoComponent::caculateGizmoMatrix()
{
    if (_attachedNodeList.empty()) return;
    if (_attachedNodeList.size() <= 1){
        auto mat = _attachedNodeList[0]->getNodeToWorldMatrix();
        veVec3 pos; veQuat rot;
        mat.decomposition(&pos, nullptr, &rot);
        _gizmoNode->setMatrix(veMat4::transform(pos, veVec3::UNIT_SCALE, rot));
    }else{
        veVec3 position;
        for (auto &attachedNode : _attachedNodeList){
            auto mat = attachedNode->getNodeToWorldMatrix();
            veVec3 nodePos;
            mat.decomposition(&nodePos, nullptr, nullptr);
            position += nodePos;
        }
        position /= _attachedNodeList.size();
        _gizmoNode->setMatrix(veMat4::translation(position));
    }
}

void veGizmoComponent::applyGizmoMatrix(const veVec3 &trans, const veVec3 &scl, const veQuat &rot)
{
    if (_attachedNodeList.empty()) return;
    _gizmoNode->setMatrix(_gizmoNode->getMatrix() * veMat4::transform(trans, veVec3::UNIT_SCALE, rot));
    //_gizmoNode->setMatrix(_gizmoNode->getMatrix() * offsetMat);
    
    if (_attachedNodeList.size() <= 1) {
        veVec3 npos, nscl; veQuat nrot;
        _attachedNodeList[0]->getMatrix().decomposition(&npos, &nscl, &nrot);
        _attachedNodeList[0]->setMatrix(veMat4::transform(nrot * rot * trans + npos, nscl + scl, nrot * rot));
    }else{
        for (auto &node : _attachedNodeList) {
            veVec3 npos, nscl; veQuat nrot;
            node->getMatrix().decomposition(&npos, &nscl, &nrot);
            node->setMatrix(veMat4::transform(trans + npos, nscl + scl, nrot * rot));
        }
    }
}

void veGizmoComponent::setGizmoType(GizmoType type)
{
    if (_type == type) return;
    _type = type;
    _refresh = true;
}