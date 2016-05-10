#include "LinearForceAffector.h"
#include "ParticleSystem.h"
#include "KernelCore/Node.h"

veLinearForceAffector::veLinearForceAffector()
{
}

veLinearForceAffector::~veLinearForceAffector()
{
    
}

void veLinearForceAffector::preProcess(double deltaTime)
{
    veBaseForceAffector::preProcess(deltaTime);
    _scaledVector = _forceVector * deltaTime;
}

void veLinearForceAffector::update(veParticle *particle, double deltaTime)
{
    if (_forceApplication == ForceApplication::FA_ADD){
        particle->direction += _scaledVector;
    }else{
        particle->direction = (particle->direction + _forceVector) / 2.0f;
    }
}