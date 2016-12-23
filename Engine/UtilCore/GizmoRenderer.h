#ifndef _VE_GIZMO_RENDERER_
#define _VE_GIZMO_RENDERER_
#include "KernelCore/Renderer.h"
#include <map>
#include <mutex>

class VE_EXPORT veGizmoRenderer : public veRenderer
{
public:

    veGizmoRenderer();
    virtual ~veGizmoRenderer();
    virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID) override;
    virtual void draw(veRenderCommand &command) override;
    
    void drawLines(const veVec3 &start, const veVec3 &end, const veVec4 &color);
    void drawTri(const veVec3 &v0, const veVec3 &v1, const veVec3 &v2, const veVec4 &color);
    void drawQuat(const veVec3 &bl, const veVec3 &br, const veVec3 &tr, const veVec3 &tl, const veVec4 &color);
    void drawCircle(const veVec3 &axes, veReal radius, const veVec4 &color);
    void drawCone(const veVec3 &pos, const veVec3 &axes, veReal radius, veReal height, const veVec4 &color);
    void drawBox(const veVec3 &pos, veReal halfSize, const veVec4 &color);
    void refresh();
    
protected:
    
    inline unsigned int getVertexStride() const;
    
protected:
    
#define LINES_INDICES     0
#define TRIANGLES_INDICES 1
#define INDICES_COUNT     2
    
    VE_Ptr<veGLDataBuffer> _vaoBuffer;
    VE_Ptr<veGLDataBuffer> _vboBuffer;
    VE_Ptr<veGLDataBuffer> _iboBuffers[INDICES_COUNT];
    veRealArray     _vertices;
    veUint16Array   _indices[INDICES_COUNT];
    std::mutex      _dataMutex;
    bool            _refresh;
};

#endif
