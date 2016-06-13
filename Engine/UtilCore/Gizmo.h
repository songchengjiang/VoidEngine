#ifndef _VE_GIZMO_
#define _VE_GIZMO_
#include "KernelCore/RenderableObject.h"
#include "KernelCore/Ray.h"

class VE_EXPORT veGizmo : public veRenderableObject
{
public:
    
    enum class AxesType{
        AT_NONE,
        AT_X,
        AT_Y,
        AT_Z,
        AT_YZ,
        AT_XZ,
        AT_XY,
        AT_XYZ,
    };
    
    veGizmo(veSceneManager *sm);
    virtual ~veGizmo();
    
    virtual void update(veNode *node, veSceneManager *sm) override;
    
    void setScale(veReal scl) { _scale = scl; _isDirtyBoundingBox = true; }
    veReal getScale() const { return _scale; }
    
    virtual AxesType touchDown(const veVec2 &screenCoords) = 0;
    virtual void touchMove(AxesType at, const veVec2 &screenCoords, veVec3 &pos, veVec3 &scl, veQuat &rot) = 0;
    virtual void touchUp() = 0;
    
protected:
    
    VE_Ptr<veRay> _ray;
    veVec3        _latestPosition;
    veVec3        _planeNormal;
    veReal        _scale;
};

#endif
