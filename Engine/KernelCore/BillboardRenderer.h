#ifndef _VE_BILLBOARD_RENDERER_
#define _VE_BILLBOARD_RENDERER_
#include "SurfaceRenderer.h"
#include "BaseCore/Array.h"

class VE_EXPORT veBillboardRenderer: public veSurfaceRenderer
{
public:
	veBillboardRenderer();
	~veBillboardRenderer();

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) override;
};

#endif
