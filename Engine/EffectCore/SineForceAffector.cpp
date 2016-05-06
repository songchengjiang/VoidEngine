#include "SineForceAffector.h"
#include "ParticleSystem.h"
#include "KernelCore/Node.h"

veSineForceAffector::veSineForceAffector()
    : _frequencyMinimum(1.f)
    , _frequencyMaximun(1.f)
    , _frequency(1.f)
    , _angle(0.f)
{
}

veSineForceAffector::~veSineForceAffector()
{
    
}

void veSineForceAffector::setFrequencyMinimum(float min)
{
    _frequencyMinimum = min;
    generateFrequency();
}

void veSineForceAffector::setFrequencyMaximun(float max)
{
    _frequencyMaximun = max;
    generateFrequency();
}

void veSineForceAffector::preProcess(double deltaTime)
{
    veBaseForceAffector::preProcess(deltaTime);
    
    _angle += _frequency * deltaTime;
    float val = veMath::veSin(_angle);
    _scaledVector = _forceVector * deltaTime * val;
    
    if (veMath::TWO_PI < _angle){
        _angle = 0.f;
        generateFrequency();
    }
}

void veSineForceAffector::update(veParticle *particle, double deltaTime)
{
    if (_forceApplication == FA_ADD){
        particle->direction += _scaledVector;
    }else{
        particle->direction = (particle->direction + _forceVector) / 2.0f;
    }
}

void veSineForceAffector::generateFrequency()
{
    if (_frequencyMinimum != _frequencyMaximun){
        veReal t = veMath::veRandomUnitization();
        _frequency = _frequencyMinimum * (1.0f - t) + _frequencyMaximun * t;
    }else{
        _frequency = _frequencyMinimum;
    }
}