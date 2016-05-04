#ifndef _VE_AFFECTOR_
#define _VE_AFFECTOR_
#include "Prerequisites.h"
#include "BaseCore/Value.h"

class veParticleSystem;
struct veParticle;
class VE_EXPORT veAffector
{
public:
    
    USE_VE_PTR;
    USE_NAME_PROPERTY;
    
    virtual ~veAffector();
    
    virtual void prepare(){};
    virtual void unPrepare(){};
    virtual void notifyStart(){};
    virtual void notifyStop(){};
    virtual void notifyPause(){};
    virtual void notifyResume(){};
    virtual void notifyRescaled(const veVec3 &scale){};
    virtual void preProcess(double deltaTime){};
    virtual void postProcess(double deltaTime){};
    virtual void firstUpdate(veParticle *particle, double deltaTime){};
    virtual void update(veParticle *particle, double deltaTime){};
    void process(veParticle *particle, float deltaTime, bool firstPartilce);
    
    
    void setParticleSystem(veParticleSystem *system) { _system = system; }
    
    void setEnabled(bool isEnabled) { _isEnabled = isEnabled; }
    bool isEnabled() const { return _isEnabled; }
    
    void setPosition(const veVec3 &pos) { _position = pos; }
    const veVec3& getPosition() const { return _position; }
    
protected:
    
    veAffector();
    
protected:
    
    veParticleSystem *_system;
    veVec3            _position;
    bool              _isEnabled;
};


#endif