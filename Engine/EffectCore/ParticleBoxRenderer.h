#ifndef _VE_PARTICLE_BOX_RENDERER_
#define _VE_PARTICLE_BOX_RENDERER_
#include "ParticleRenderer.h"

class veParticle;
class VE_EXPORT veParticleBoxRenderer : public veParticleRenderer
{
public:
    
    veParticleBoxRenderer();
    virtual ~veParticleBoxRenderer();
    
    virtual void draw(veRenderCommand &command) override;
    
protected:
    
    virtual void updateBuffer(veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID) override;
    void updateInstanceParams(veParticle *particle,const veMat4 &vp);
    inline unsigned int getVertexStride() const;
};

#endif
