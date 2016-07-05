#ifndef _VE_GIZMO_COMPONENT_
#define _VE_GIZMO_COMPONENT_
#include "KernelCore/component.h"
#include "GizmoTranslation.h"
#include "GizmoRotation.h"
#include "GizmoScale.h"
#include "KernelCore/Node.h"

class VE_EXPORT veGizmoComponent : public veComponent
{
public:
    enum class GizmoType{
        GT_TRANSLATION,
        GT_ROTATION,
        GT_SCALE
    };
    
    veGizmoComponent();
    virtual ~veGizmoComponent();
    
    virtual bool handle(veSceneManager *sm, veViewer *viewer, const veEvent &event) override;
    virtual void afterUpdate(veSceneManager *sm) override;
    virtual void beforeRender(veSceneManager *sm, veViewer *viewer) override;
    virtual bool onAttachToNode(veNode *node) override;
    virtual bool onDetachToNode(veNode *node) override;
    
    void setGizmoType(GizmoType type);
    GizmoType getGizmoType() const { return _type; }
    
protected:
    
    void caculateGizmoMatrix();
    void applyGizmoMatrix(const veVec3 &trans, const veVec3 &scl, const veQuat &rot);
    
protected:
    
    GizmoType      _type;
    VE_Ptr<veNode> _gizmoNode;
    VE_Ptr<veGizmo> _gizmo;
    veGizmo::AxesType _gizmoAxesType;
    bool            _refresh;
};

#endif
