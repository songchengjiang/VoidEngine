#include "ScaleVelocityAffector.h"
#include "Particle.h"

veScaleVelocityAffector::veScaleVelocityAffector()
{
    _scale = new veValueFixed;
    static_cast<veValueFixed *>(_scale.get())->setValue(1.0f);
}

veScaleVelocityAffector::~veScaleVelocityAffector()
{
    
}

void veScaleVelocityAffector::update(veParticle *particle, double deltaTime)
{
    particle->velocity += _scale->getValue(particle->timeFraction) * deltaTime;
}