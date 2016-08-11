#ifndef _VE_SURFACE_RENDERER_
#define _VE_SURFACE_RENDERER_
#include "Renderer.h"
#include "BaseCore/Array.h"

class VE_EXPORT veSurfaceRenderer : public veRenderer
{
public:
	veSurfaceRenderer();
	~veSurfaceRenderer();

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID) override;
	void draw(veRenderCommand &command) override;

protected:

	void updateBuffer(unsigned int contextID);

protected:

    VE_Ptr<veGLDataBuffer> _vaoBuffer;
    VE_Ptr<veGLDataBuffer> _vboBuffer;
	veRealArray     _vertices;
	bool            _needRefresh;
};

#endif
