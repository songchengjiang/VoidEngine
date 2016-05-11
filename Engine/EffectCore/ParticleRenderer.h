#ifndef _VE_PARTICLE_RENDERER_
#define _VE_PARTICLE_RENDERER_
#include "BaseCore/Array.h"
#include "KernelCore/Renderer.h"

#define INSTANCE_MAX_NUM 100

class veParticleSystem;
class VE_EXPORT veParticleRenderer : public veRenderer
{
public:
    veParticleRenderer();
    virtual ~veParticleRenderer();
    
    virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) override;
    virtual void draw(veRenderCommand &command) override;
    
    virtual void update();
    
protected:
    
    virtual void updateBuffer(veRenderableObject *renderableObj, veCamera *camera) = 0;
    
protected:
    
    GLuint          _vao;
    GLuint          _vbo;
    GLuint          _ibo;
    veRealArray     _vertices;
    veUint16Array   _indices;
    bool            _needUpdate;
    veParticleSystem *_system;
    VE_Ptr<veUniform> _offsetMatsUniform;
    VE_Ptr<veUniform> _colorsUniform;
    veMat4            _offsetMats[INSTANCE_MAX_NUM];
    veVec4            _colors[INSTANCE_MAX_NUM];
};

#endif
