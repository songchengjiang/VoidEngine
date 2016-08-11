#include "SphereEmitter.h"
#include "ParticleSystem.h"

veSphereEmitter::veSphereEmitter()
    : _radius(5.f)
{
    
}

veSphereEmitter::~veSphereEmitter()
{
    
}

void veSphereEmitter::initParticlePosition(veParticle* particle)
{
#define RANDOM_MINUS1_1() (veMath::veRandomUnitization() * 2.0f - 1.0f)
    
    veVec3 randomDir = veVec3(RANDOM_MINUS1_1(), RANDOM_MINUS1_1(), RANDOM_MINUS1_1());
    randomDir.normalize();
    
    particle->position = _worldPosition + _worldRotation * randomDir * _radius;
    particle->originalPosition = particle->position;
}