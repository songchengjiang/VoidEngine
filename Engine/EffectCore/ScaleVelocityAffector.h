#ifndef _VE_SCALE_VELOCITY_AFFECTOR_
#define _VE_SCALE_VELOCITY_AFFECTOR_
#include "Affector.h"
#include "BaseCore/Value.h"
#include "KernelCore/VE_Ptr.h"

class VE_EXPORT veScaleVelocityAffector : public veAffector
{
public:
    
    veScaleVelocityAffector();
    virtual ~veScaleVelocityAffector();
    
    void setScaleVelocity(veValue *val) { _scale = val; }
    const veValue* getScaleVelocity() const { return _scale.get(); }
    
    virtual void update(veParticle *particle, double deltaTime) override;
    
protected:
    
    VE_Ptr<veValue> _scale;
};

#endif