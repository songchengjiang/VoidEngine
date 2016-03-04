#ifndef _VE_DEFERRED_LIGHT_PIPELINE_
#define _VE_DEFERRED_LIGHT_PIPELINE_
#include "RenderPipeline.h"

class veDeferredLightPipeline : public veRenderPipeline
{
public:
	veDeferredLightPipeline(veSceneManager *sm);
	virtual ~veDeferredLightPipeline();

protected:

	virtual void renderScene(veCamera *camera, bool isMainCamera) override;

};

#endif