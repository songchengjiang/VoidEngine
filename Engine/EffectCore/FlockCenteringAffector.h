#ifndef _VE_FLOCK_CENTERING_AFFECTOR_
#define _VE_FLOCK_CENTERING_AFFECTOR_
#include "Affector.h"

class VE_EXPORT veFlockCenteringAffector : public veAffector
{
public:
    
    veFlockCenteringAffector();
    virtual ~veFlockCenteringAffector();
    
    virtual void preProcess(double deltaTime) override;
    virtual void update(veParticle *particle, double deltaTime) override;
    
protected:
    
    veVec3 _sum;
    veVec3 _average;
    unsigned int _count;
    
};

#endif