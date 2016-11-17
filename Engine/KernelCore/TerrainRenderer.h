#ifndef _VE_TERRAIN_RENDERER_
#define _VE_TERRAIN_RENDERER_
#include "Renderer.h"
#include "BaseCore/Array.h"

class VE_EXPORT veTerrainRenderer : public veRenderer
{
public:
	veTerrainRenderer();
	~veTerrainRenderer();

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID) override;
	void draw(veRenderCommand &command) override;

protected:

    VE_Ptr<veGLDataBuffer> _vaoBuffer;
    VE_Ptr<veGLDataBuffer> _vboBuffer;
    VE_Ptr<veGLDataBuffer> _iboBuffer;
    GLsizei                _indicesCount;
	bool                   _needRefresh;
};

#endif
