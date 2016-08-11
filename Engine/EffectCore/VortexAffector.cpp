#include "VortexAffector.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "KernelCore/Node.h"

veVortexAffector::veVortexAffector()
{
    _rotationSpeed = new veValueFixed;
    static_cast<veValueFixed *>(_rotationSpeed.get())->setValue(1.0f);
}

veVortexAffector::~veVortexAffector()
{
    
}

void veVortexAffector::preProcess(double deltaTime)
{
    auto node = _system->getParents()[0];
    auto nTow = node->getNodeToWorldMatrix() * veMat4::transform(_position, veVec3::UNIT_SCALE, _rotation);
    nTow.decomposition(&_worldPosition, nullptr, &_worldRotation);
    _vortexRotation.set(_rotationSpeed->getValue(_system->getTimeElapsedSinceStart()) * deltaTime, _worldRotation * veVec3::UNIT_Y);
}

void veVortexAffector::update(veParticle *particle, double deltaTime)
{
    veVec3 local = particle->position - _worldPosition;
    particle->position = _worldPosition + _vortexRotation * local;
    particle->direction = _vortexRotation * particle->direction;
    particle->orientation = _vortexRotation * particle->orientation;
}