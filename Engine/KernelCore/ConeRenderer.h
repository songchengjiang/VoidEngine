#ifndef _VE_CONE_RENDERER_
#define _VE_CONE_RENDERER_
#include "Renderer.h"
#include "BaseCore/Array.h"

class VE_EXPORT veConeRenderer : public veRenderer
{
public:
	veConeRenderer();
	~veConeRenderer();

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) override;
	void draw(const veRenderCommand &command);

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