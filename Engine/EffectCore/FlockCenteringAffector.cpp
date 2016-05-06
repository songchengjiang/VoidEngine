#include "FlockCenteringAffector.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "KernelCore/Node.h"

veFlockCenteringAffector::veFlockCenteringAffector()
    : _count(0)
{
}

veFlockCenteringAffector::~veFlockCenteringAffector()
{
    
}

void veFlockCenteringAffector::preProcess(double deltaTime)
{
    if (_count != 0){
        _average = _sum / _count;
    }else{
        auto node = _system->getParents()[0];
        auto nTow = node->getNodeToWorldMatrix() * veMat4::transform(_position, veVec3::UNIT_SCALE, _rotation);
        nTow.decomposition(&_average, nullptr, nullptr);
    }
    _sum = veVec3::ZERO;
    _count = 0;
}

void veFlockCenteringAffector::update(veParticle *particle, double deltaTime)
{
    _sum += particle->position;
    ++_count;
    particle->direction += (_average - particle->position) * deltaTime;
}