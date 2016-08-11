#include "JetAffector.h"
#include "Particle.h"

veJetAffector::veJetAffector()
    : _scaled(0.f)
{
    _acceleration = new veValueFixed;
    static_cast<veValueFixed *>(_acceleration.get())->setValue(1.0f);
}

veJetAffector::~veJetAffector()
{
    
}


void veJetAffector::update(veParticle *particle, double deltaTime)
{
    _scaled = deltaTime * _acceleration->getValue(particle->timeFraction);
    if (particle->direction == veVec3::ZERO){
        particle->direction += particle->originalDirection * _scaled;
    }else{
        particle->direction += particle->direction * _scaled;
    }
}