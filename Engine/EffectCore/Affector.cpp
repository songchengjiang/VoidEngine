#include "Affector.h"
#include "ParticleSystem.h"

veAffector::veAffector()
    : USE_VE_PTR_INIT
    , _system(nullptr)
    , _isEnabled(true)
{
    
}

veAffector::~veAffector()
{
    
}

void veAffector::process(veParticle *particle, float deltaTime, bool firstPartilce)
{
    if (firstPartilce){
        firstUpdate(particle, deltaTime);
    }
    
    update(particle, deltaTime);
}