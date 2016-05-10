#ifndef _VE_CIRCLE_EMITTER_
#define _VE_CIRCLE_EMITTER_
#include "Emitter.h"

class VE_EXPORT veCircleEmitter : public veEmitter
{
public:
    veCircleEmitter();
    virtual ~veCircleEmitter();
    
    void setRadius(veReal radius) { _radius = radius; }
    veReal getRadius() const { return _radius; }
    
    void setStepAngle(veReal stepAngle) { _stepAngle = stepAngle; _circleAngle = 0.0f; }
    veReal getStepAngle() const { return _stepAngle; }
    
    void setUseRandomAngle(bool isUseRandomAng) { _isUseRandomAng = isUseRandomAng; }
    bool isUsedRandomAngle() const { return _isUseRandomAng; }
    
protected:
    
    virtual void initParticlePosition(veParticle* particle) override;
    
protected:
    
    veReal _radius;
    veReal _stepAngle;
    veReal _circleAngle;
    bool   _isUseRandomAng;
};

#endif
