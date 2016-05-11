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
    
    veQuat cameraRot;
    const veMat4 &camWorldMat = command.camera->getNodeToWorldMatrix();
    camWorldMat.decomposition(nullptr, nullptr, &cameraRot);
    
    glBindVertexArray(_vao);
    const auto &particles = _system->getParticles().getActiveDataList();
    unsigned int instanceNum = 0;
    for (auto particle : particles){
        updateInstanceParams(particle, cameraRot, _offsetMats[instanceNum], _colors[instanceNum]);
        ++instanceNum;
        if (INSTANCE_MAX_NUM <= instanceNum){
            _offsetMatsUniform->setValue(_offsetMats, instanceNum);
            _offsetMatsUniform->apply(command);
            _colorsUniform->setValue(_colors, instanceNum);
            _colorsUniform->apply(command);
            glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, instanceNum);
            instanceNum = 0;
        }
    }
    
    if (0 < instanceNum){
        _offsetMatsUniform->setValue(_offsetMats, instanceNum);
        _offsetMatsUniform->apply(command);
        _colorsUniform->setValue(_colors, instanceNum);
        _colorsUniform->apply(command);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, instanceNum);
    }
}

void veQuatRenderer::updateInstanceParams(veParticle *particle, const veQuat &cameraRot, veMat4 &offsetMat, veVec4 &color)
{
    veVec3 scale(particle->width, particle->height, 1.0f);
    offsetMat.makeTransform(particle->position, veVec3(particle->width, particle->height, 1.0f), cameraRot * particle->orientation);
    color = particle->color;
}

void veQuatRenderer::updateBuffer(veRenderableObject *renderableObj, veCamera *camera)
{
    if (!_vertices.empty()){
        if (!_vao) {
            glGenVertexArrays(1, &_vao);
            glGenBuffers(1, &_vbo);
            glGenBuffers(1, &_ibo);
        }
        glBindVertexArray(_vao);
        if (!_vertices.empty()){
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(_vertices[0]), _vertices.buffer(), GL_STATIC_DRAW);
        }
        
        unsigned int stride = sizeof(GLfloat) * getVertexStride();
        //v
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
        glEnableVertexAttribArray(0);
        
        //tc
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(sizeof(GLfloat) * 3));
        glEnableVertexAttribArray(1);
    }
    
}

unsigned int veQuatRenderer::getVertexStride() const
{
    return 3 + 2;
}