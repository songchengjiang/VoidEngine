#ifndef _VE_MESH_RENDERER_
#define _VE_MESH_RENDERER_
#include "Renderer.h"
#include "BaseCore/Array.h"
#include <unordered_map>

class veMesh;
class veNode;
class VE_EXPORT veMeshRenderer : public veRenderer
{
public:

	veMeshRenderer();
	~veMeshRenderer();

    virtual void render(veRenderableObject *renderableObj, veCamera *camera, const veMat4 &worldMatrix, unsigned int contextID, veRenderQueue::RenderQueueType type) override;
	virtual void draw(veRenderCommand &command) override;

    void needRefresh(){ _needRefresh = true; }
protected:

    VE_Ptr<veGLDataBuffer> _vboBuffer;
    std::vector< VE_Ptr<veGLDataBuffer> > _iboBuffers;
    std::unordered_map<unsigned int, std::string> _attributesNamesMap;
	GLenum _drawUsage;
    veMesh *_mesh;
    bool    _needRefresh;
};

#endif
