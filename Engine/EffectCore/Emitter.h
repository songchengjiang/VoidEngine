#ifndef _VE_EMITTER_
#define _VE_EMITTER_
#include "Prerequisites.h"
#include "BaseCore/Value.h"
#include "KernelCore/VE_Ptr.h"

class veParticleSystem;
struct veParticle;
class VE_EXPORT veEmitter
{
public:
    
    USE_VE_PTR;
    USE_NAME_PROPERTY;
    
    virtual ~veEmitter();
    
    virtual void prepare();
    virtual void unPrepare(){};
    virtual void notifyStart(){};
    virtual void notifyStop(){};
    virtual void notifyPause(){};
    virtual void notifyResume(){};
    virtual void notifyRescaled(const veVec3 &scale){};
    virtual void preProcess(double deltaTime);
    virtual void postProcess(double deltaTime);
    
    virtual void initParticleForEmission(veParticle* particle);
    virtual unsigned short caculateRequestedParticles(double timeElapsed);
    
    void setParticleSystem(veParticleSystem *system) { _system = system; }
    
    void setPosition(const veVec3 &pos) { _position = pos; }
    const veVec3& getPosition() const { return _position; }
    
    void setRotation(const veQuat &rot) { _rotation = rot; }
    const veQuat& getRotation() { return _rotation; }
    
    void setEnabled(bool isEnabled) { _isEnabled = isEnabled; initTimeBase(); }
    bool isEnabled() const { return _isEnabled; }
    
    void keepLocalParticles(bool isKeep) { _isKeepLocalParticles = isKeep; }
    bool isKeepLocalParticles() const { return _isKeepLocalParticles; }
    
    void setAngle(veValue *value) { _angle = value; }
    veValue* getAngle() const { return _angle.get(); }
    
    void setEmission(veValue *value) { _emissionRate = value; }
    veValue* getEmission() const { return _emissionRate.get(); }
    
    void setTotalTimeToLive(veValue *value) { _totalTimeToLive = value; }
    veValue* getTotalTimeToLive() const { return _totalTimeToLive.get(); }
    
    void setMass(veValue *value) { _mass = value; }
    veValue* getMass() const { return _mass.get(); }
    
    void setVelocity(veValue *value) { _velocity = value; }
    veValue* getVelocity() const { return _velocity.get(); }
    
    void setDuration(veValue *value) { _duration = value; initTimeBase();  }
    veValue* getDuration() const { return _duration.get(); }
    
    void setRepeatDelay(veValue *value) { _repeatDelay = value; initTimeBase();  }
    veValue* getRepeatDelay() const { return _repeatDelay.get(); }
    
    void setColor(const veVec4 &col) { _color = col; }
    const veVec4& getColor() const { return _color; }
    
    void setOrientation(const veQuat &start, const veQuat &end);
    void getOrientation(veQuat &start, veQuat &end);
    
    void setWidth(veValue *value) { _width = value; }
    veValue* getWidth() const { return _width.get(); }
    
    void setHeight(veValue *value) { _height = value; }
    veValue* getHeight() const { return _height.get(); }
    
    void setDepth(veValue *value) { _depth = value; }
    veValue* getDepth() const { return _depth.get(); }

    void makeLocalParticles(veParticle* particle);
    
protected:
    
    veEmitter();
    
    virtual void initParticlePosition(veParticle* particle);
    virtual void initParticleDirection(veParticle* particle);
    virtual void initParticleOrientation(veParticle* particle);
    virtual void initParticleVelocity(veParticle* particle);
    virtual void initParticleMass(veParticle* particle);
    virtual void initParticleColor(veParticle* particle);
    virtual void initParticleTextureCoords(veParticle* particle);
    virtual float initParticleTimeToLive();
    virtual void initParticleDimensions(veParticle* particle);
    
    void initTimeBase();
    void generateAngle(veReal &angle);
    void reCalulate();
    
protected:
    
    veParticleSystem *_system;
    veVec3            _position;
    veVec3            _worldPosition;
    veVec3            _preWorldPosition;
    veVec3            _worldUp;
    veQuat            _rotation;
    veQuat            _worldRotation;
    veQuat            _orientationStart;
    veQuat            _orientationEnd;
    veVec4            _color;
    bool              _isEnabled;
    bool              _isKeepLocalParticles;
    
    double            _remainder;
    double            _durationRemain;
    double            _repeatDelayRemain;
    
    VE_Ptr<veValue>   _angle;
    VE_Ptr<veValue>   _emissionRate;
    VE_Ptr<veValue>   _totalTimeToLive;
    VE_Ptr<veValue>   _mass;
    VE_Ptr<veValue>   _velocity;
    VE_Ptr<veValue>   _duration;
    VE_Ptr<veValue>   _repeatDelay;
    VE_Ptr<veValue>   _width;
    VE_Ptr<veValue>   _height;
    VE_Ptr<veValue>   _depth;
    
};

#endif
