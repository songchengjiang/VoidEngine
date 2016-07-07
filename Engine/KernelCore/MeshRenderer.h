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

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) override;
	virtual void draw(veRenderCommand &command) override;

    void needRefresh(){ _needRefresh = true; }
protected:

    GLuint _vao;
    GLuint _vbo;
    std::vector<GLuint> _ibos;
	GLenum _drawUsage;
    veMesh *_mesh;
    bool    _needRefresh;
};

#endif
