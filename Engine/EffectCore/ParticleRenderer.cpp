#include "ParticleRenderer.h"
#include "KernelCore/Node.h"
#include "KernelCore/Camera.h"
#include "KernelCore/RenderQueue.h"

veParticleRenderer::veParticleRenderer()
    : _vao(0)
    , _vbo(0)
{

}

veParticleRenderer::~veParticleRenderer()
{

}

void veParticleRenderer::draw(veRenderCommand &command)
{
    if (!isNeedRendering())
        return;
    if (!command.pass->apply(command))
        return;
    
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, GLsizei(_indices.size()), GL_UNSIGNED_SHORT, nullptr);
}

void veParticleRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera)
{
    if (_needUpdate){
        updateBuffer(renderableObj, camera);
        _needUpdate = false;
    }
    
    veRenderCommand rc;
    rc.mask = node->getMask();
    rc.worldMatrix = new veMat4Ptr(veMat4::IDENTITY);
    rc.camera = camera;
    rc.sceneManager = camera->getSceneManager();
    rc.depthInCamera = (camera->viewMatrix() * rc.worldMatrix->value())[2][3];
    rc.renderer = this;
    
    auto materials = renderableObj->getMaterialArray();
    for (unsigned int mat = 0; mat < materials->getMaterialNum(); ++mat) {
        auto material = materials->getMaterial(mat);
        for (unsigned int i = 0; i < material->activeTechnique()->getPassNum(); ++i) {
            auto pass = material->activeTechnique()->getPass(i);
            if (camera->getMask() & pass->drawMask()) {
                bool isTransparent = pass->blendFunc() != veBlendFunc::DISABLE ? true : false;
                rc.pass = pass;
                pass->visit(rc);
                if (isTransparent)
                    camera->getRenderQueue()->pushCommand(i, veRenderQueue::RENDER_QUEUE_TRANSPARENT, rc);
                else
                    camera->getRenderQueue()->pushCommand(i, veRenderQueue::RENDER_QUEUE_ENTITY, rc);
            }
        }
    }
}

void veParticleRenderer::update()
{
    _needUpdate = true;
}