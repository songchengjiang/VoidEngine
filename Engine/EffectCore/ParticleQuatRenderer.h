#ifndef _VE_PARTICLE_QUAT_RENDERER_
#define _VE_PARTICLE_QUAT_RENDERER_
#include "ParticleRenderer.h"

class veParticle;
class VE_EXPORT veParticleQuatRenderer : public veParticleRenderer
{
public:
    
    enum class OrientationType{
        OT_BILLBOARD,
        OT_DIRECTION,
        OT_FIXED,
    };
    veParticleQuatRenderer();
    virtual ~veParticleQuatRenderer();
    
    virtual void draw(veRenderCommand &command) override;
    
    void setOrientationType(OrientationType type) { _orientationType = type; }
    OrientationType getOrientationType() const { return _orientationType; }
    
protected:
    
    virtual void updateBuffer(veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID) override;
    void updateInstanceParams(veParticle *particle, const veMat4 &cameraRot, const veMat4 &vp);
    void addParticle(const veVec3 &v0, const veVec3 &v1, const veVec3 &v2, const veVec3 &v3
                   , const veVec4 &color);
    inline unsigned int getVertexStride() const;

protected:
    
    OrientationType _orientationType;
};

#endif
