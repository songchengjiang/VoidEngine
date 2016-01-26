#ifndef _VE_DEFERRED_LIGHT_PIPELINE_
#define _VE_DEFERRED_LIGHT_PIPELINE_
#include "RenderPipeline.h"

class DeferredLightPipeline : public RenderPipeline
{
public:
	DeferredLightPipeline(veSceneManager *sm);
	virtual ~DeferredLightPipeline();

protected:

	virtual void renderCamera(veCamera *camera) override;

};

#endif