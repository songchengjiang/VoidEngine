#ifndef _VE_PARTICLE_RENDERER_
#define _VE_PARTICLE_RENDERER_
#include "BaseCore/Array.h"
#include "KernelCore/Renderer.h"

class veParticleSystem;
class VE_EXPORT veParticleRenderer : public veRenderer
{
public:
    veParticleRenderer();
    virtual ~veParticleRenderer();
    
    virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID) override;
    virtual void draw(veRenderCommand &command) override;
    
    virtual void update();
    
protected:
    
    virtual void updateBuffer(veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID) = 0;
    
protected:
    
    VE_Ptr<veGLDataBuffer> _vaoBuffer;
    VE_Ptr<veGLDataBuffer> _vboBuffer;
    VE_Ptr<veGLDataBuffer> _iboBuffer;
    VE_Ptr<veGLDataBuffer> _mvpboBuffer;
    VE_Ptr<veGLDataBuffer> _colorboBuffer;
    veRealArray     _vertices;
    veUint16Array   _indices;
    bool            _needUpdate;
    veArray<veMat4>   _mvpMats;
    veArray<veVec4>   _colors;
    unsigned int      _instanceCount;
};

#endif
