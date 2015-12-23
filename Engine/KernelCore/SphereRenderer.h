#ifndef _VE_SPHERE_RENDERER_
#define _VE_SPHERE_RENDERER_
#include "Renderer.h"
#include "BaseCore/Array.h"

class VE_EXPORT veSphereRenderer : public veRenderer
{
public:
	veSphereRenderer();
	~veSphereRenderer();

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) override;
	void draw(veRenderCommand &command) override;

protected:

	void updateBuffer();

protected:

	GLuint          _vao;
	GLuint          _vbo;
	GLuint          _ibo;
	veRealArray     _vertices;
	veUint16Array     _indices;
	bool            _needRefresh;
};

#endif