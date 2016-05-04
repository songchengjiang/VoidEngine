#include "Emitter.h"
#include "ParticleSystem.h"
#include "KernelCore/Node.h"

static const veVec4 DEFAULT_COLOR = veVec4::WHITE;
static const float DEFAULT_EMISSION_RATE = 10.0f;
static const float DEFAULT_TIME_TO_LIVE = 3.0f;
static const float DEFAULT_MASS = 1.0f;
static const float DEFAULT_VELOCITY = 100.0f;
static const float DEFAULT_DURATION = 0.0f;
static const float DEFAULT_REPEAT_DELAY = 0.0f;
static const float DEFAULT_ANGLE = 20.0f;
static const float DEFAULT_WIDTH = 1.0f;
static const float DEFAULT_HEIGHT = 1.0f;
static const float DEFAULT_DEPTH = 1.0f;

#define GET_VALUE(VAL, X, DEF_VAL) VAL? VAL->getValue(X): DEF_VAL

veEmitter::veEmitter()
    : USE_VE_PTR_INIT
    , _isEnabled(true)
    , _isKeepLocalParticles(false)
    , _color(DEFAULT_COLOR)
    , _remainder(0.0)
    , _durationRemain(0.0)
    , _repeatDelayRemain(0.0)
{
    _angle = new veValueFixed;
    static_cast<veValueFixed *>(_angle.get())->setValue(DEFAULT_ANGLE);
    _emissionRate = new veValueFixed;
    static_cast<veValueFixed *>(_emissionRate.get())->setValue(DEFAULT_EMISSION_RATE);
    _totalTimeToLive = new veValueFixed;
    static_cast<veValueFixed *>(_totalTimeToLive.get())->setValue(DEFAULT_TIME_TO_LIVE);
    _mass = new veValueFixed;
    static_cast<veValueFixed *>(_mass.get())->setValue(DEFAULT_MASS);
    _velocity = new veValueFixed;
    static_cast<veValueFixed *>(_velocity.get())->setValue(DEFAULT_VELOCITY);
    _duration = new veValueFixed;
    static_cast<veValueFixed *>(_duration.get())->setValue(DEFAULT_DURATION);
    _repeatDelay = new veValueFixed;
    static_cast<veValueFixed *>(_repeatDelay.get())->setValue(DEFAULT_REPEAT_DELAY);
    _width = new veValueFixed;
    static_cast<veValueFixed *>(_width.get())->setValue(DEFAULT_WIDTH);
    _height = new veValueFixed;
    static_cast<veValueFixed *>(_height.get())->setValue(DEFAULT_HEIGHT);
    _depth = new veValueFixed;
    static_cast<veValueFixed *>(_depth.get())->setValue(DEFAULT_DEPTH);
}

veEmitter::~veEmitter()
{

}

void veEmitter::prepare()
{
    reCalulate();
    _preWorldPosition = _worldPosition;
}

void veEmitter::preProcess(double deltaTime)
{
    _preWorldPosition = _worldPosition;
    reCalulate();
}

void veEmitter::postProcess(double deltaTime)
{

}

void veEmitter::reCalulate()
{
    auto node = _system->getParents()[0];
    auto nTow = node->getNodeToWorldMatrix() * veMat4::transform(_position, veVec3::UNIT_SCALE, _rotation);
    nTow.decomposition(&_worldPosition, nullptr, &_worldRotation);
    
    veVec3 dir = _worldRotation * veVec3::UNIT_Y;
    _worldUp = dir.perpendicular();
}

void veEmitter::makeLocalParticles(veParticle* particle)
{
    if (!_isKeepLocalParticles) return;
    
    particle->position += _worldPosition - _preWorldPosition;
}

unsigned short veEmitter::caculateRequestedParticles(double timeElapsed)
{
    unsigned short requestedParticles = 0;
    
    if (_isEnabled)
    {
        if (_emissionRate.valid())
        {
            float rate = _emissionRate->getValue(_system->getTimeElapsedSinceStart());
            {
                // Particle emission is time driven
                _remainder += rate * timeElapsed;
                requestedParticles = (unsigned short)_remainder;
            }
            
            _remainder -= requestedParticles;
        }
        
        // Determine whether the duration period has been exceeded.
        if (_duration->getValue(_system->getTimeElapsedSinceStart()) != 0.f)
        {
            _durationRemain -= timeElapsed;
            if (_durationRemain <= 0.f)
            {
                setEnabled(false);
            }
        }
    }
    else if (_repeatDelay->getValue(_system->getTimeElapsedSinceStart()) != 0.f)
    {
        _repeatDelayRemain -= timeElapsed;
        if (_repeatDelayRemain <= 0.f)
        {
            setEnabled(true);
        }
    }
    
    return requestedParticles;

}

void veEmitter::initParticleForEmission(veParticle* particle)
{
    initParticlePosition(particle);
    initParticleDirection(particle);
    initParticleVelocity(particle);
    initParticleOrientation(particle);
    initParticleMass(particle);
    initParticleColor(particle);
    initParticleTextureCoords(particle);
    particle->totalTimeToLive = initParticleTimeToLive();
    particle->timeToLive = particle->totalTimeToLive;
    particle->emitter = this;
    
    initParticleDimensions(particle);
}

void veEmitter::initParticlePosition(veParticle* particle)
{
    particle->position         = _worldPosition;
    particle->originalPosition = particle->position;
}

void veEmitter::initParticleDirection(veParticle* particle)
{
    veReal angle = 0.f;
    generateAngle(angle);
    veVec3 dir = _worldRotation * veVec3::UNIT_Y;
    if (angle != 0.f){
        particle->direction = dir.randomDeviat(angle, _worldUp);
    }else {
        particle->direction = dir;
    }
    
    particle->originalDirection = particle->direction;
}

void veEmitter::initParticleOrientation(veParticle* particle)
{
    particle->orientation = veQuat::nlerp(veMath::veRandomUnitization(), _orientationStart, _orientationEnd);
    particle->originalOrientation = particle->orientation;
}

void veEmitter::initParticleVelocity(veParticle* particle)
{
    particle->velocity = GET_VALUE(_velocity.get(), _system->getTimeElapsedSinceStart(), 1.f);
    particle->originalVelocity = particle->velocity;
}

void veEmitter::initParticleMass(veParticle* particle)
{
    particle->mass = GET_VALUE(_mass.get(), _system->getTimeElapsedSinceStart(), DEFAULT_MASS);
}

void veEmitter::initParticleColor(veParticle* particle)
{
    particle->color = _color;
}

void veEmitter::initParticleTextureCoords(veParticle* particle)
{
    
}

float veEmitter::initParticleTimeToLive()
{
    return GET_VALUE(_totalTimeToLive.get(), _system->getTimeElapsedSinceStart(), DEFAULT_TIME_TO_LIVE);
}

void veEmitter::initParticleDimensions(veParticle* particle)
{
    particle->width = GET_VALUE(_width.get(), _system->getTimeElapsedSinceStart(), DEFAULT_WIDTH);
    particle->height = GET_VALUE(_height.get(), _system->getTimeElapsedSinceStart(), DEFAULT_HEIGHT);
    particle->depth = GET_VALUE(_depth.get(), _system->getTimeElapsedSinceStart(), DEFAULT_DEPTH);
}

void veEmitter::initTimeBase()
{
    if (_isEnabled){
        _durationRemain = GET_VALUE(_duration.get(), _system->getTimeElapsedSinceStart(), 0.f);
        if (0.f < _durationRemain){
            _repeatDelayRemain = 0.f;
        }
    }else {
        _repeatDelayRemain = GET_VALUE(_repeatDelay.get(), _system->getTimeElapsedSinceStart(), 0.f);
    }
}

void veEmitter::setOrientation(const veQuat &start, const veQuat &end)
{
    _orientationStart = start;
    _orientationEnd = end;
}

void veEmitter::getOrientation(veQuat &start, veQuat &end)
{
    start = _orientationStart;
    end   = _orientationEnd;
}

void veEmitter::generateAngle(veReal &angle)
{
    angle = veMath::veRadian(GET_VALUE(_angle.get(), _system->getTimeElapsedSinceStart(), 0.f));
    if (_angle->getType() == veValue::Type::VAL_FIXED){
        angle = veMath::veRandomUnitization() * angle;
    }
}