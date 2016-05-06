#include "LineEmitter.h"
#include "ParticleSystem.h"

veLineEmitter::veLineEmitter()
    : _lineSegments(0)
    , _segments(0)
    , _start(veVec3(-5.f, 0.f, 0.f))
    , _end(veVec3(5.f, 0.f, 0.f))
    , _isUseRandomRange(true)
{
    
}

veLineEmitter::~veLineEmitter()
{
    
}

void veLineEmitter::setStart(const veVec3 &start)
{
    _start = start;
    _lineStep = (_end - _start).length() / _lineSegments;
}

void veLineEmitter::setEnd(const veVec3 &end)
{
    _end = end;
    _lineStep = (_end - _start).length() / _lineSegments;
}

void veLineEmitter::veLineEmitter::setLineSegments(unsigned int segments)
{
    _lineSegments = segments;
    _lineStep = (_end - _start).length() / _lineSegments;
}

void veLineEmitter::initParticlePosition(veParticle* particle)
{
    veVec3 range = 0.0f;
    if (_isUseRandomRange){
        veReal t = veMath::veRandomUnitization();
        range = _start * (1.0f - t) + _end * t;
    }else{
        range = _start + _segments * _lineStep;
        ++_segments;
        _segments = _lineSegments < _segments? 0: _segments;
    }
    
    particle->position = _worldPosition + _worldRotation * range;
    particle->originalPosition = particle->position;
}