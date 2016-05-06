#ifndef _VE_JET_AFFECTOR_
#define _VE_JET_AFFECTOR_
#include "Affector.h"
#include "BaseCore/Value.h"
#include "KernelCore/VE_Ptr.h"

class VE_EXPORT veJetAffector : public veAffector
{
public:
    
    veJetAffector();
    virtual ~veJetAffector();
    
    void setAcceleration(veValue *acceleration) { _acceleration = acceleration; }
    const veValue* getAcceleration() const { return _acceleration.get(); }
    
    virtual void update(veParticle *particle, double deltaTime) override;
    
protected:
    
    veReal _scaled;
    VE_Ptr<veValue> _acceleration;
    
};

#endif