#ifndef _VE_SPHERE_EMITTER_
#define _VE_SPHERE_EMITTER_
#include "Emitter.h"

class VE_EXPORT veSphereEmitter : public veEmitter
{
public:
    veSphereEmitter();
    virtual ~veSphereEmitter();
    
    void setRadius(veReal radius) { _radius = radius; }
    veReal getRadius() const { return _radius; }
    
protected:
    
    virtual void initParticlePosition(veParticle* particle) override;
    
protected:
    
    veReal _radius;
};

#endif
