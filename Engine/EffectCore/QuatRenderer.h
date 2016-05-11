#ifndef _VE_QUAT_RENDERER_
#define _VE_QUAT_RENDERER_
#include "ParticleRenderer.h"

class veParticle;
class VE_EXPORT veQuatRenderer : public veParticleRenderer
{
public:
    veQuatRenderer();
    virtual ~veQuatRenderer();
    
    virtual void draw(veRenderCommand &command) override;
    
protected:
    
    virtual void updateBuffer(veRenderableObject *renderableObj, veCamera *camera) override;
    void updateInstanceParams(veParticle *particle, const veQuat &cameraRot, veMat4 &offsetMat, veVec4 &color);    
    inline unsigned int getVertexStride() const;

};

#endif
