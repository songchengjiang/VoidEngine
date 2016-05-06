#ifndef _VE_VORTEX_AFFECTOR_
#define _VE_VORTEX_AFFECTOR_
#include "Affector.h"
#include "BaseCore/Value.h"
#include "KernelCore/VE_Ptr.h"

class VE_EXPORT veVortexAffector : public veAffector
{
public:
    
    veVortexAffector();
    virtual ~veVortexAffector();
    
    void setRotationSpeed(veValue *speed) { _rotationSpeed = speed; }
    const veValue* getRotationSpeed() const { return _rotationSpeed.get(); }
    
    virtual void preProcess(double deltaTime) override;
    virtual void update(veParticle *particle, double deltaTime) override;
    
protected:
    
    VE_Ptr<veValue> _rotationSpeed;
    veVec3          _worldPosition;
    veQuat          _worldRotation;
    veQuat          _vortexRotation;
};

#endif