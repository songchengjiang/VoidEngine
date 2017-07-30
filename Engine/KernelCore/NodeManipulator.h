#ifndef _VE_NODE_MANIPULATOR_
#define _VE_NODE_MANIPULATOR_
#include "Component.h"
#include "BaseCore/Vector2.h"
#include "BaseCore/Vector3.h"
#include "BaseCore/Quaternion.h"

class veNodeManipulator : public veComponent
{
public:
    veNodeManipulator();
    ~veNodeManipulator();
    
    virtual void start(veSceneManager *sm) override;
    virtual void update(veSceneManager *sm) override;
    virtual bool handle(veSceneManager *sm, veViewer *viewer, const veEvent &event) override;
    
private:
    
    void rotateNode(const veVec2 &g0, const veVec2 &g1);
    void zoomNode(float zoomDelta);
    void moveNode(const veVec2 &g0, const veVec2 &g1);
    
    void updateMatrix();
    void trackball(veVec3 & axis, float & angle, float p1x, float p1y, float p2x, float p2y);
    float tb_project_to_sphere(float r, float x, float y);
    
    
private:
    
    veQuat _rotation;
    veVec3 _scale;
    veVec3 _translation;
    veVec3 _localTranslation;
    veVec2 _g0;
    veVec2 _g1;
    veReal _defaultCameraDistance;
    veReal _defaultCameraZoomScale;
    double _simulationTime;
    double _latesTouchTime;
};

#endif
