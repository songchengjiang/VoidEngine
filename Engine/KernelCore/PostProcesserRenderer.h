#ifndef _VE_POST_PROCESSER_RENDERER_
#define _VE_POST_PROCESSER_RENDERER_
#include "SurfaceRenderer.h"
#include "BaseCore/Array.h"

class VE_EXPORT vePostProcesserRenderer : public veSurfaceRenderer
{
public:
	vePostProcesserRenderer();
	~vePostProcesserRenderer();

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) override;
	void setPostProcessingPass(vePass *pass) { _pass = pass; }

private:
	VE_Ptr<vePass> _pass;
};

#endif
