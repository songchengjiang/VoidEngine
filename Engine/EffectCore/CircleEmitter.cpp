#include "CircleEmitter.h"
#include "ParticleSystem.h"

veCircleEmitter::veCircleEmitter()
    : _radius(5.0f)
    , _stepAngle(0.f)
    , _circleAngle(0.f)
    , _isUseRandomAng(true)
{

}

veCircleEmitter::~veCircleEmitter()
{

}

void veCircleEmitter::initParticlePosition(veParticle* particle)
{
    veReal angle = 0.0f;
    if (_isUseRandomAng){
        angle = veMath::veRandomUnitization() * veMath::TWO_PI;
    }else{
        angle = _circleAngle;
        _circleAngle += _stepAngle;
        _circleAngle = veMath::TWO_PI < _circleAngle? _circleAngle - veMath::TWO_PI: _circleAngle;
    }
    
    veReal x = veMath::veCos(angle);
    veReal z = veMath::veSin(angle);
    
    particle->position = _worldPosition + _worldRotation * veVec3(x * _radius, 0.0f, z * _radius);
    particle->originalPosition = particle->position;
}