#ifndef _VE_GRAVITY_AFFECTOR_
#define _VE_GRAVITY_AFFECTOR_
#include "Affector.h"

class VE_EXPORT veGravityAffector : public veAffector
{
public:
    
    veGravityAffector();
    virtual ~veGravityAffector();
    
    void setGravity(veReal gravity) { _gravity = gravity; }
    veReal getGravity() const { return _gravity; }
    
    virtual void preProcess(double deltaTime) override;
    virtual void update(veParticle *particle, double deltaTime) override;
    
protected:
    
    veReal _gravity;
    veVec3 _worldPosition;
};

#endif