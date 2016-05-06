#include "GravityAffector.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "KernelCore/Node.h"

veGravityAffector::veGravityAffector()
    : _gravity(1.f)
{
}

veGravityAffector::~veGravityAffector()
{
    
}

void veGravityAffector::preProcess(double deltaTime)
{
    auto node = _system->getParents()[0];
    auto nTow = node->getNodeToWorldMatrix() * veMat4::transform(_position, veVec3::UNIT_SCALE, _rotation);
    nTow.decomposition(&_worldPosition, nullptr, nullptr);
}

void veGravityAffector::update(veParticle *particle, double deltaTime)
{
    veVec3 distance = _worldPosition - particle->position;
    veReal length = distance.squaredLength();
    if (0.f < length){
        veReal force = (_gravity * particle->mass) / length;
        particle->direction += force * distance * deltaTime;
    }
}
