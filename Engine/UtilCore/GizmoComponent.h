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
    virtual void update(veSceneManager *sm) override;
    virtual void beforeRender(veSceneManager *sm, veViewer *viewer) override;
    
    void setGizmoType(GizmoType type);
    GizmoType getGizmoType() const { return _type; }
    
    void addHandleNode(veNode *node);
    void removeHandleNode(veNode *node);
    
protected:
    
    void caculateGizmoMatrix();
    void applyGizmoMatrix(const veVec3 &trans, const veVec3 &scl, const veQuat &rot);
    void updateGizmo(const veMat4 &mat);
    
protected:
    
    veNodeList      _handleNodeList;
    GizmoType       _type;
    VE_Ptr<veGizmo> _gizmo;
    veGizmo::AxesType _gizmoAxesType;
    bool            _refresh;
};

#endif
