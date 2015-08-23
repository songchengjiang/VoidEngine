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

	virtual void render() override;

protected:

	void renderMesh(veMesh *mesh);

protected:

	GLuint _vao;
	GLuint _vbo;
	std::vector<GLuint> _ibos;
};

#endif
