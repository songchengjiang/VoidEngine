#ifndef _VE_OVERLAY_RENDERER_
#define _VE_OVERLAY_RENDERER_
#include "Renderer.h"
#include "BaseCore/Array.h"

class VE_EXPORT veOverlayRenderer : public veRenderer
{
public:
	veOverlayRenderer();
	~veOverlayRenderer();

	virtual void visit(veNode *node, veRenderableObject *renderableObj, veVisualiser *vs) override;
	virtual void render(const veRenderCommand &command) override;

protected:

	GLuint          _vao;
	GLuint          _vbo;
	veRealArray     _vertices;
	bool            _needRefresh;
};

#endif
