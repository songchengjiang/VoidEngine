#include "ParticleBoxRenderer.h"
#include "KernelCore/Node.h"
#include "KernelCore/Camera.h"
#include "KernelCore/RenderQueue.h"
#include "ParticleSystem.h"
#include "Particle.h"

#define VERTEX_BUFFER_SIZE 40
#define INDEX_BUFFER_SIZE 36

static double box_vertices[VERTEX_BUFFER_SIZE] = {
    -0.500000,-0.500000,0.500000,0.000000,0.750000
    ,0.500000,-0.500000,0.500000,0.000000,0.250000
    ,0.500000,0.500000,0.500000,0.500000,0.250000
    ,-0.500000,0.500000,0.500000,0.500000,0.750000
    ,0.500000,-0.500000,-0.500000,0.000000,0.000000
    ,-0.500000,-0.500000,-0.500000,0.000000,1.000000
    ,-0.500000,0.500000,-0.500000,0.500000,1.000000
    ,0.500000,0.500000,-0.500000,0.500000,0.000000
};

static uint16_t box_indices[INDEX_BUFFER_SIZE] = {
    0,2,3,0,1,2,1,7,2,1,4,7,4,6,7,4,5,6,5,3,6,5,0,3
    ,3,7,6,3,2,7,5,1,0,5,4,1
};

veParticleBoxRenderer::veParticleBoxRenderer()
{
    for (unsigned int i = 0; i < VERTEX_BUFFER_SIZE; i += 5) {
        _vertices.push_back(box_vertices[i + 0]); _vertices.push_back(box_vertices[i + 1]); _vertices.push_back(box_vertices[i + 2]); //v
        _vertices.push_back(box_vertices[i + 3]); _vertices.push_back(box_vertices[i + 4]);                              //tc
    }

    for (unsigned int face = 0; face < INDEX_BUFFER_SIZE; face += 3) {
        _indices.push_back(box_indices[face + 0]); _indices.push_back(box_indices[face + 1]); _indices.push_back(box_indices[face + 2]);
    }
}

veParticleBoxRenderer::~veParticleBoxRenderer()
{

}

void veParticleBoxRenderer::draw(veRenderCommand &command)
{
    
    if (!command.pass->apply(command))
        return;
    
//    glBindVertexArray(_vaoBuffer->getData(command.contextID));
//    glDrawElementsInstanced(GL_TRIANGLES, GLsizei(_indices.size()), GL_UNSIGNED_SHORT, nullptr, _instanceCount);
}

void veParticleBoxRenderer::updateInstanceParams(veParticle *particle, const veMat4 &vp)
{
    veMat4 mat = vp * veMat4::transform(particle->position, veVec3(particle->width, particle->height, particle->depth), particle->orientation);
    mat.transpose();
//    _mvpMats.push_back(mat);
//    _colors.push_back(particle->color);
}

void veParticleBoxRenderer::updateBuffer(veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID)
{
//    auto vao = _vaoBuffer->getData(contextID);
//    if (!vao) {
//        vao = _vaoBuffer->createData(contextID);
//        _needUpdate = true;
//    }
//    if (!_vertices.empty() && !_indices.empty() && _needUpdate){
//
//        auto vbo = _vboBuffer->getData(contextID);
//        if (!vbo){
//            vbo = _vboBuffer->createData(contextID);
//        }
//
//        auto ibo = _iboBuffer->getData(contextID);
//        if (!ibo){
//            ibo = _iboBuffer->createData(contextID);
//        }
//
//        auto mvpbo = _mvpboBuffer->getData(contextID);
//        if (!mvpbo){
//            mvpbo = _mvpboBuffer->createData(contextID);
//        }
//
//        auto colorbo = _colorboBuffer->getData(contextID);
//        if (!colorbo){
//            colorbo = _colorboBuffer->createData(contextID);
//        }
//        glBindVertexArray(vao);
//        glBindBuffer(GL_ARRAY_BUFFER, vbo);
//        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(_vertices[0]), _vertices.buffer(), GL_STATIC_DRAW);
//
//        unsigned int stride = sizeof(GLfloat) * getVertexStride();
//        //v
//        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
//        glEnableVertexAttribArray(0);
//
//        //tc
//        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(sizeof(GLfloat) * 3));
//        glEnableVertexAttribArray(1);
//
//        glBindBuffer(GL_ARRAY_BUFFER, colorbo);
//        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
//        glVertexAttribDivisor(2, 1);
//        glEnableVertexAttribArray(2);
//
//        glBindBuffer(GL_ARRAY_BUFFER, mvpbo);
//        for (unsigned short i = 0; i < 4; ++i){
//            glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(veMat4),
//                                  (GLvoid*)(sizeof(GLfloat) * i * 4));
//            glVertexAttribDivisor(3 + i, 1);
//            glEnableVertexAttribArray(3 + i);
//        }
//
//        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
//        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(_indices[0]), _indices.buffer(), GL_STATIC_DRAW);
//
//        _needUpdate = false;
//    }
//
//    _instanceCount = 0;
//    _mvpMats.clear();
//    _colors.clear();
//    veMat4 viewProjMat = camera->projectionMatrix() * camera->viewMatrix();
//
//    const auto &particles = static_cast<veParticleSystem *>(renderableObj)->getParticles().getActiveDataList();
//    static_cast<veParticleSystem *>(renderableObj)->lock();
//    for (auto particle : particles){
//        updateInstanceParams(particle, viewProjMat);
//        ++_instanceCount;
//    }
//    static_cast<veParticleSystem *>(renderableObj)->unLock();
//
//    if (!_colors.empty()){
//        auto colorbo = _colorboBuffer->getData(contextID);
//        glBindBuffer(GL_ARRAY_BUFFER, colorbo);
//        glBufferData(GL_ARRAY_BUFFER, _colors.size() * sizeof(_colors[0]), _colors.buffer(), GL_DYNAMIC_DRAW);
//    }
//
//    if (!_mvpMats.empty()){
//        auto mvpbo = _mvpboBuffer->getData(contextID);
//        glBindBuffer(GL_ARRAY_BUFFER, mvpbo);
//        glBufferData(GL_ARRAY_BUFFER, _mvpMats.size() * sizeof(_mvpMats[0]), _mvpMats.buffer(), GL_DYNAMIC_DRAW);
//    }
}

unsigned int veParticleBoxRenderer::getVertexStride() const
{
    return 3 + 2;
}
