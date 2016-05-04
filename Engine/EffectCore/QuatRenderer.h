#ifndef _VE_QUAT_RENDERER_
#define _VE_QUAT_RENDERER_
#include "ParticleRenderer.h"

class veParticle;
class VE_EXPORT veQuatRenderer : public veParticleRenderer
{
public:
    veQuatRenderer();
    virtual ~veQuatRenderer();
    
protected:
    
    virtual void updateBuffer(veRenderableObject *renderableObj, veCamera *camera) override;
    
    void addQuatParticleToBuffer(unsigned int idx, veParticle *particle, const veVec3 &right, const veVec3 &up, const veVec3 &backward);
    
    inline unsigned int getVertexStride() const;

};

#endif
