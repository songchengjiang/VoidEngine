#ifndef _VE_BOX_RENDERER_
#define _VE_BOX_RENDERER_
#include "Renderer.h"
#include "BaseCore/Array.h"

class VE_EXPORT veBoxRenderer : public veRenderer
{
public:
	veBoxRenderer();
	~veBoxRenderer();

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
