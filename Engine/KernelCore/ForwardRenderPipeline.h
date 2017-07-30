#ifndef _VE_FORWARD_RENDER_PIPELINE_
#define _VE_FORWARD_RENDER_PIPELINE_
#include "RenderPipeline.h"
#include "FrameBufferObject.h"
#include "Texture.h"
class VE_EXPORT veForwardRenderPipeline : public veRenderPipeline
{
public:
    veForwardRenderPipeline(veSceneManager *sm);
    ~veForwardRenderPipeline();
    
protected:
    
    virtual void renderScene(veCamera *camera, unsigned int contextID) override;
};

#endif
