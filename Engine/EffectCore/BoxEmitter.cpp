#include "BoxEmitter.h"
#include "ParticleSystem.h"

veBoxEmitter::veBoxEmitter()
    : _boxWidth(10.f)
    , _boxHeight(10.0f)
    , _boxDepth(10.0f)
{

}

veBoxEmitter::~veBoxEmitter()
{

}

void veBoxEmitter::initParticlePosition(veParticle* particle)
{
#define RANDOM_MINUS1_1() (veMath::veRandomUnitization() * 2.0f - 1.0f)
    
    particle->position = _worldPosition + _worldRotation * veVec3(RANDOM_MINUS1_1() * _boxWidth * 0.5f
                                                                , RANDOM_MINUS1_1() * _boxHeight * 0.5f
                                                                , RANDOM_MINUS1_1() * _boxDepth * 0.5f);
    
    particle->originalPosition = particle->position;
}