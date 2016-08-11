#ifndef _VE_PARTICLE_
#define _VE_PARTICLE_
#include "Prerequisites.h"
#include "BaseCore/Vector3.h"
#include "BaseCore/Vector4.h"
#include "BaseCore/Quaternion.h"
#include "BaseCore/DataPool.h"

class veEmitter;
class VE_EXPORT veParticle
{
public:
    
    enum ReservedParticleEventFlags
    {
        PEF_EXPIRED  = 1<<0,
        PEF_EMITTED  = 1<<1,
        PEF_COLLIDED = 1<<2,
    };
    
    inline void setEventFlags(unsigned int flags) {eventFlags = flags;}
    inline void addEventFlags(unsigned int flags) {eventFlags |= flags;}
    inline void removeEventFlags(unsigned int flags) {eventFlags &= ~flags;}
    inline unsigned int getEventFlags() const {return eventFlags;}
    inline bool hasEventFlags(unsigned int flags) const {return (eventFlags & flags) != 0;}
    
    inline void process(){
        timeFraction = (totalTimeToLive - timeToLive) / (totalTimeToLive);
    }
    
    inline bool isExpired(double deltaTime){
        bool expired = timeToLive < deltaTime;
        if (expired){
            addEventFlags(PEF_EXPIRED);
        }
        return expired;
    }
    
    unsigned int eventFlags;
    
    veEmitter *emitter;
    veReal originalVelocity;
    veReal velocity;
    veVec3 originalPosition;
    veVec3 position;
    veVec3 originalDirection;
    veVec3 direction;
    veQuat originalOrientation;
    veQuat orientation;
    
    veVec4 originalColor;
    veVec4 color;
  
    
    double timeToLive;
    double totalTimeToLive;
    double timeFraction;
    float  mass;
    
    float width;
    float height;
    float depth;
};

typedef veDataPool<veParticle> ParticlePool;

#endif
