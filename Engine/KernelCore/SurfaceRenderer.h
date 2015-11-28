#ifndef _VE_SURFACE_RENDERER_
#define _VE_SURFACE_RENDERER_
#include "Renderer.h"
#include "BaseCore/Array.h"

class VE_EXPORT veSurfaceRenderer : public veRenderer
{
public:
	veSurfaceRenderer();
	~veSurfaceRenderer();

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) override;
	void draw(const veRenderCommand &command) override;

protected:

	void updateBuffer();

protected:

	GLuint          _vao;
	GLuint          _vbo;
	veRealArray     _vertices;
	bool            _needRefresh;
};

#endif
