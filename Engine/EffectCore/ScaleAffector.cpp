#include "ScaleAffector.h"
#include "Particle.h"

veScaleAffector::veScaleAffector()
{
    _scaleX = new veValueFixed;
    static_cast<veValueFixed *>(_scaleX.get())->setValue(0.0f);
    _scaleY = new veValueFixed;
    static_cast<veValueFixed *>(_scaleY.get())->setValue(0.0f);
    _scaleZ = new veValueFixed;
    static_cast<veValueFixed *>(_scaleZ.get())->setValue(0.0f);
}

veScaleAffector::~veScaleAffector()
{

}

void veScaleAffector::update(veParticle *particle, double deltaTime)
{
    particle->width += _scaleX->getValue(particle->timeFraction) * deltaTime;
    particle->height += _scaleY->getValue(particle->timeFraction) * deltaTime;
    particle->depth += _scaleZ->getValue(particle->timeFraction) * deltaTime;
    
    particle->width = veMath::maximum(0.0f, particle->width);
    particle->height = veMath::maximum(0.0f, particle->height);
    particle->depth = veMath::maximum(0.0f, particle->depth);
}