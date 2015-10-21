#ifndef _VE_ENTITY_RENDERER_
#define _VE_ENTITY_RENDERER_
#include "Renderer.h"
#include "BaseCore/Array.h"

class veMesh;
class veNode;
class VE_EXPORT veEntityRenderer : public veRenderer
{
public:
	struct MeshBuffers
	{
		MeshBuffers()
			: node(nullptr)
			, mesh(nullptr)
			, vao(0)
			, vbo(0){

		}
		veNode *node;
		veMesh *mesh;
		GLuint vao;
		GLuint vbo;
		std::vector<GLuint> ibos;
	};

	veEntityRenderer();
	~veEntityRenderer();

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) override;
	void draw(const veRenderCommand &command);

protected:

	std::vector<MeshBuffers> _meshBuffersList;
	GLenum _drawUsage;
};

#endif
