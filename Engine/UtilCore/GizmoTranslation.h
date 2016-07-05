#ifndef _VE_GIZMO_TRANSLATION_
#define _VE_GIZMO_TRANSLATION_
#include "Gizmo.h"

class VE_EXPORT veGizmoTranslation : public veGizmo
{
public:
    
    veGizmoTranslation(veSceneManager *sm);
    virtual ~veGizmoTranslation();
    
    virtual bool intersectWith(veRay *ray, veNode *node) override;
    
    virtual AxesType touchDown(veViewer *viewer, const veVec2 &screenCoords) override;
    virtual void touchMove(veViewer *viewer, AxesType at, const veVec2 &screenCoords, veVec3 &pos, veVec3 &scl, veQuat &rot) override;
    virtual void touchUp() override;
    
protected:

    void drawAxes(const veVec4 &axesXCol, const veVec4 &axesYCol, const veVec4 &axesZCol, const veVec4 &YZCol, const veVec4 &XZCol, const veVec4 &XYCol);
};

#endif
