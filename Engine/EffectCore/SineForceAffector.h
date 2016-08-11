#ifndef _VE_SINE_FORCE_AFFECTOR_
#define _VE_SINE_FORCE_AFFECTOR_
#include "BaseForceAffector.h"

class VE_EXPORT veSineForceAffector : public veBaseForceAffector
{
public:
    
    veSineForceAffector();
    virtual ~veSineForceAffector();
    
    virtual void preProcess(double deltaTime) override;
    virtual void update(veParticle *particle, double deltaTime) override;
    
    void setFrequencyMinimum(float min);
    float getFrequencyMinimum() const { return _frequencyMinimum; }
    
    void setFrequencyMaximun(float max);
    float getFrequencyMaximun() const { return _frequencyMaximun; }
    
protected:
    
    void generateFrequency();
    
protected:
    
    float _frequencyMinimum;
    float _frequencyMaximun;
    float _frequency;
    veReal _angle;
};

#endif