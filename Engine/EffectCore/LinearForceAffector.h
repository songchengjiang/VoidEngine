#ifndef _VE_LINEAR_FORCE_AFFECTOR_
#define _VE_LINEAR_FORCE_AFFECTOR_
#include "BaseForceAffector.h"

class VE_EXPORT veLinearForceAffector : public veBaseForceAffector
{
public:
    
    veLinearForceAffector();
    virtual ~veLinearForceAffector();
    
    virtual void preProcess(double deltaTime) override;
    virtual void update(veParticle *particle, double deltaTime) override;
};

#endif