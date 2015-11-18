#ifndef _VE_ENTITY_RENDERER_
#define _VE_ENTITY_RENDERER_
#include "Renderer.h"
#include "BaseCore/Array.h"

class veMesh;
class veNode;
class VE_EXPORT veEntityRenderer : public veRenderer
{
public:

	veEntityRenderer();
	~veEntityRenderer();

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) override;

protected:

	struct MeshBuffers
	{
		MeshBuffers()
			: vao(0)
			, vbo(0) {

		}

		GLuint vao;
		GLuint vbo;
		std::vector<GLuint> ibos;
	};

	std::vector<MeshBuffers> _meshBuffersList;
	GLenum _drawUsage;
};

#endif
