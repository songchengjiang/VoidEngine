#ifndef _VE_MESH_RENDERER_
#define _VE_MESH_RENDERER_
#include "Renderer.h"
#include "BaseCore/Array.h"

class veMesh;
class VE_EXPORT veMeshRenderer : public veRenderer
{
public:
	veMeshRenderer();
	~veMeshRenderer();

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) override;
	virtual void draw(const veRenderCommand &command) override;
	virtual void uniformUpdate(veUniform *uniform, const veRenderCommand &command);

protected:

	GLuint _vao;
	GLuint _vbo;
	GLenum _drawUsage;
	std::vector<GLuint> _ibos;
};

#endif
