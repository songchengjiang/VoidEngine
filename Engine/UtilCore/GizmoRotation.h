#ifndef _VE_GIZMO_ROTATION_
#define _VE_GIZMO_ROTATION_
#include "Gizmo.h"

class VE_EXPORT veGizmoRotation : public veGizmo
{
public:
    
    veGizmoRotation(veSceneManager *sm);
    virtual ~veGizmoRotation();
    
    virtual bool intersectWith(veRay *ray, veNode *node) override;
    
    virtual AxesType touchDown(veViewer *viewer, const veVec2 &screenCoords) override;
    virtual void touchMove(veViewer *viewer, AxesType at, const veVec2 &screenCoords, veVec3 &pos, veVec3 &scl, veQuat &rot) override;
    virtual void touchUp() override;
    
protected:
    
    void drawAxes(const veVec4 &YZCol, const veVec4 &XZCol, const veVec4 &XYCol);
};

#endif
