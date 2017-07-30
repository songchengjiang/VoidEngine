#ifndef _VE_CAMERA_MANIPULATOR_
#define _VE_CAMERA_MANIPULATOR_
#include "Component.h"
#include "BaseCore/Vector2.h"
#include "BaseCore/Vector3.h"
#include "BaseCore/Quaternion.h"

class veCameraManipulator : public veComponent
{
public:
    veCameraManipulator();
    ~veCameraManipulator();
    
    virtual void start(veSceneManager *sm) override;
    virtual void update(veSceneManager *sm) override;
    virtual bool handle(veSceneManager *sm, veViewer *viewer, const veEvent &event) override;
    
private:
    
    void rotateCamera(const veVec2 &g0, const veVec2 &g1);
    void moveCamera(const veVec2 &g0, const veVec2 &g1);
    void zoomCamera(float zoomDelta);
    void resetCamera();
    void updateViewMatrix();
    void trackball(veVec3 & axis, float & angle, float p1x, float p1y, float p2x, float p2y);
    float tb_project_to_sphere(float r, float x, float y);
    
    
private:
    
    veQuat _rotation;
    veVec3 _center;
    veVec3 _translate;
    veVec2 _g0;
    veVec2 _g1;
    veReal _defaultCameraDistance;
    veReal _defaultCameraZoomScale;
    double _simulationTime;
    double _latesTouchTime;
};

#endif
