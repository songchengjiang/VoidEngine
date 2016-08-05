#ifndef _VE_MESH_RENDERER_
#define _VE_MESH_RENDERER_
#include "Renderer.h"
#include "BaseCore/Array.h"

class veMesh;
class veNode;
class VE_EXPORT veMeshRenderer : public veRenderer
{
public:

	veMeshRenderer();
	~veMeshRenderer();

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID) override;
	virtual void draw(veRenderCommand &command) override;

    void needRefresh(){ _needRefresh = true; }
protected:

    VE_Ptr<veGLDataBuffer> _vaoBuffer;
    VE_Ptr<veGLDataBuffer> _vboBuffer;
    std::vector< VE_Ptr<veGLDataBuffer> > _iboBuffers;
	GLenum _drawUsage;
    veMesh *_mesh;
    bool    _needRefresh;
};

#endif
