#ifndef _VE_POST_PROCESSER_RENDERER_
#define _VE_POST_PROCESSER_RENDERER_
#include "SurfaceRenderer.h"
#include "BaseCore/Array.h"

class VE_EXPORT vePostProcesserRenderer : public veSurfaceRenderer
{
public:
	vePostProcesserRenderer();
	~vePostProcesserRenderer();

	void render(veNode *node, vePass *pass, veCamera *camera);
};

#endif
