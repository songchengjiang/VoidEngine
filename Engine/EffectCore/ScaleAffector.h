#ifndef _VE_SCALE_AFFECTOR_
#define _VE_SCALE_AFFECTOR_
#include "Affector.h"
#include "BaseCore/Value.h"
#include "KernelCore/VE_Ptr.h"

class VE_EXPORT veScaleAffector : public veAffector
{
public:
    
    veScaleAffector();
    virtual ~veScaleAffector();
    
    void setScaleX(veValue *val) { _scaleX = val; }
    const veValue* getScaleX() const { return _scaleX.get(); }
    void setScaleY(veValue *val) { _scaleY = val; }
    const veValue* getScaleY() const { return _scaleY.get(); }
    void setScaleZ(veValue *val) { _scaleZ = val; }
    const veValue* getScaleZ() const { return _scaleZ.get(); }
    
    virtual void update(veParticle *particle, double deltaTime) override;
    
protected:
    
    VE_Ptr<veValue> _scaleX;
    VE_Ptr<veValue> _scaleY;
    VE_Ptr<veValue> _scaleZ;
};

#endif