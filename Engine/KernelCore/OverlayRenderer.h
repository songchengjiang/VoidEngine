#ifndef _VE_OVERLAY_RENDERER_
#define _VE_OVERLAY_RENDERER_
#include "SurfaceRenderer.h"
#include "BaseCore/Array.h"

class VE_EXPORT veOverlayRenderer : public veSurfaceRenderer
{
public:
	veOverlayRenderer();
	~veOverlayRenderer();

	void setRenderOrder(int order) { _renderOrder = order; }
	int getRenderOrder() const { return _renderOrder; }

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) override;

protected:

	int _renderOrder;
};

#endif
