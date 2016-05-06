#ifndef _VE_BOX_EMITTER_
#define _VE_BOX_EMITTER_
#include "Emitter.h"

class VE_EXPORT veBoxEmitter : public veEmitter
{
public:
    veBoxEmitter();
    virtual ~veBoxEmitter();
    
    void setBoxWidth(veReal width) { _boxWidth = width; }
    veReal getBoxWidth() const { return _boxWidth; }
    void setBoxHeight(veReal height) { _boxHeight = height; }
    veReal getBoxHeight() const { return _boxHeight; }
    void setBoxDepth(veReal depth) { _boxDepth = depth; }
    veReal getBoxDepth() const { return _boxDepth; }
    
protected:
    
    virtual void initParticlePosition(veParticle* particle) override;
    
protected:
    
    veReal _boxWidth;
    veReal _boxHeight;
    veReal _boxDepth;
};

#endif
