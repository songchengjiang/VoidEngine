#ifndef _VE_BASE_FORCE_AFFECTOR_
#define _VE_BASE_FORCE_AFFECTOR_
#include "Affector.h"

class VE_EXPORT veBaseForceAffector : public veAffector
{
public:
    
    enum class ForceApplication{
        FA_AVERAGE,
        FA_ADD,
    };
    
    veBaseForceAffector();
    virtual ~veBaseForceAffector();
    
    virtual void preProcess(double deltaTime) override;
    
    
    void setForceScalar(veReal scalar) { _forceScalar = scalar; }
    veReal getForceScalar() const { return _forceScalar; }
    
    void setForceApplication(ForceApplication fa) { _forceApplication = fa; }
    ForceApplication getForceApplication() const { return _forceApplication; }
    
protected:
    
    ForceApplication _forceApplication;
    veVec3 _scaledVector;
    veVec3 _forceVector;
    veReal _forceScalar;
};

#endif