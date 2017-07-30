#include "ForwardRenderPipeline.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Constants.h"
#include "Configuration.h"
#include "Shaders/Light.glsl.h"

veForwardRenderPipeline::veForwardRenderPipeline(veSceneManager *sm)
: veRenderPipeline(sm)
{
}

veForwardRenderPipeline::~veForwardRenderPipeline()
{
    
}

void veForwardRenderPipeline::renderScene(veCamera *camera, unsigned int contextID)
{
    //renderLights(camera, contextID);
    
    if (camera->getFrameBufferObject())
        camera->getFrameBufferObject()->bind(contextID, camera->getClearMask(), GL_FRAMEBUFFER);
#if VE_PLATFORM == VE_PLATFORM_MAC
    prepareForDraws(camera);
#endif
    draw(camera, camera->getRenderQueue()->forwardRenderGroup);
    
//    if (!camera->getPostProcesserList().empty()) {
//        bool isFirstProcesser = true;
//        for (auto &iter : camera->getPostProcesserList()) {
//            auto processer = iter.get();
//            if (processer->isEnabled()) {
//                processer->process(this, camera, isFirstProcesser, contextID);
//                isFirstProcesser = false;
//            }
//        }
//    }
    
    if (camera->getFrameBufferObject())
        camera->getFrameBufferObject()->unBind();
    
}
