#include "RenderQueue.h"
#include "RenderCommand.h"
#include "Renderer.h"
#include "Camera.h"

veRenderQueue::veRenderQueue()
{

}

veRenderQueue::~veRenderQueue()
{

}

void veRenderQueue::pushCommand(unsigned int renderPassIndex, unsigned int renderQueueType, const veRenderCommand &cmd)
{
//    if (cmd.pass->getRenderPass() == vePass::DEFERRED_PASS){
//        auto &passRenderQueue = deferredRenderGroup[renderPassIndex];
//        passRenderQueue[renderQueueType].push_back(cmd);
//    }else{
        auto &passRenderQueue = forwardRenderGroup[renderPassIndex];
        passRenderQueue[renderQueueType].push_back(cmd);
//    }
}
