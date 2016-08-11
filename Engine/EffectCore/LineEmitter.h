#ifndef _VE_LINE_EMITTER_
#define _VE_LINE_EMITTER_
#include "Emitter.h"

class VE_EXPORT veLineEmitter : public veEmitter
{
public:
    veLineEmitter();
    virtual ~veLineEmitter();
    
    void setStart(const veVec3 &start);
    const veVec3& getStart() const { return _start; }
    void setEnd(const veVec3 &end);
    const veVec3& getEnd() const { return _end; }
    
    void setLineSegments(unsigned int segments);
    unsigned int getLineSegments() const { return _lineSegments; }
    
    void setUseRandomRange(bool isUseRandomRange) { _isUseRandomRange = isUseRandomRange; }
    bool isUsedRandomRange() const { return _isUseRandomRange; }
    
protected:
    
    virtual void initParticlePosition(veParticle* particle) override;
    
protected:
    
    veVec3 _start;
    veVec3 _end;
    unsigned int _lineSegments;
    unsigned int _segments;
    veVec3 _lineStep;
    bool   _isUseRandomRange;
};

#endif
