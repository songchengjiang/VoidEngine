#include "QuatRenderer.h"
#include "KernelCore/Node.h"
#include "KernelCore/Camera.h"
#include "KernelCore/RenderQueue.h"
#include "ParticleSystem.h"
#include "Particle.h"

veQuatRenderer::veQuatRenderer()
{
    //0
    _vertices.push_back(-0.5f); _vertices.push_back(-0.5f); _vertices.push_back(0.0f); //v
    _vertices.push_back(0.0f); _vertices.push_back(0.0f);                              //tc
    
    //1
    _vertices.push_back(0.5f); _vertices.push_back(-0.5f); _vertices.push_back(0.0f); //v
    _vertices.push_back(1.0f); _vertices.push_back(0.0f);                              //tc
    
    //2
    _vertices.push_back(-0.5f); _vertices.push_back(0.5f); _vertices.push_back(0.0f); //v
    _vertices.push_back(0.0f); _vertices.push_back(1.0f);                              //tc
    
    //3
    _vertices.push_back(0.5f); _vertices.push_back(0.5f); _vertices.push_back(0.0f); //v
    _vertices.push_back(1.0f); _vertices.push_back(1.0f);                              //tc
}

veQuatRenderer::~veQuatRenderer()
{

}

void veQuatRenderer::draw(veRenderCommand &command)
{
    if (!isNeedRendering())
        return;
    
    if (!command.pass->apply(command))
        return;
    
    glBindVertexArray(_vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, _instanceCount);
}

void veQuatRenderer::updateInstanceParams(veParticle *particle, const veQuat &cameraRot, const veMat4 &vp)
{
    veVec3 scale(particle->width, particle->height, 1.0f);
    veMat4 mat = vp * veMat4::transform(particle->position, veVec3(particle->width, particle->height, 1.0f), cameraRot * particle->orientation);
    mat.transpose();
    _mvpMats.push_back(mat);
    _colors.push_back(particle->color);
    ++_instanceCount;
}

void veQuatRenderer::updateBuffer(veRenderableObject *renderableObj, veCamera *camera)
{
    if (!_vertices.empty() && _needUpdate){
        if (!_vao) {
            glGenVertexArrays(1, &_vao);
            glGenBuffers(1, &_vbo);
            glGenBuffers(1, &_mvpbo);
            glGenBuffers(1, &_colorbo);
        }
        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(_vertices[0]), _vertices.buffer(), GL_STATIC_DRAW);
        
        unsigned int stride = sizeof(GLfloat) * getVertexStride();
        //v
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
        glEnableVertexAttribArray(0);
        
        //tc
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(sizeof(GLfloat) * 3));
        glEnableVertexAttribArray(1);
        
        glBindBuffer(GL_ARRAY_BUFFER, _colorbo);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glVertexAttribDivisor(2, 1);
        glEnableVertexAttribArray(2);
        
        glBindBuffer(GL_ARRAY_BUFFER, _mvpbo);
        for (unsigned short i = 0; i < 4; ++i){
            glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(veMat4),
                                  (GLvoid*)(sizeof(GLfloat) * i * 4));
            glVertexAttribDivisor(3 + i, 1);
            glEnableVertexAttribArray(3 + i);
        }
        
        _needUpdate = false;
    }
    
    _instanceCount = 0;
    _mvpMats.clear();
    _colors.clear();
    veQuat cameraRot;
    const veMat4 &camWorldMat = camera->getNodeToWorldMatrix();
    camWorldMat.decomposition(nullptr, nullptr, &cameraRot);
    veMat4 viewProjMat = camera->projectionMatrix() * camera->viewMatrix();
    
    const auto &particles = static_cast<veParticleSystem *>(renderableObj)->getParticles().getActiveDataList();
    for (auto particle : particles){
        updateInstanceParams(particle, cameraRot, viewProjMat);
    }
    
    if (!_colors.empty()){
        glBindBuffer(GL_ARRAY_BUFFER, _colorbo);
        glBufferData(GL_ARRAY_BUFFER, _colors.size() * sizeof(_colors[0]), _colors.buffer(), GL_DYNAMIC_DRAW);
    }
    
    if (!_mvpMats.empty()){
        glBindBuffer(GL_ARRAY_BUFFER, _mvpbo);
        glBufferData(GL_ARRAY_BUFFER, _mvpMats.size() * sizeof(_mvpMats[0]), _mvpMats.buffer(), GL_DYNAMIC_DRAW);
    }
}

unsigned int veQuatRenderer::getVertexStride() const
{
    return 3 + 2;
}