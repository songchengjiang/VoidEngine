#include "QuatRenderer.h"
#include "KernelCore/Node.h"
#include "KernelCore/Camera.h"
#include "KernelCore/RenderQueue.h"
#include "ParticleSystem.h"
#include "Particle.h"

veQuatRenderer::veQuatRenderer()
{

}

veQuatRenderer::~veQuatRenderer()
{

}

void veQuatRenderer::updateBuffer(veRenderableObject *renderableObj, veCamera *camera)
{
    const auto &particles = static_cast<veParticleSystem *>(renderableObj)->getParticles().getActiveDataList();
    
//    unsigned int vertexNumPerParticle = 4 * getVertexStride();
//    if (_vertices.size() < particles.size() * vertexNumPerParticle){
//        _vertices.resize(particles.size() * vertexNumPerParticle);
//    }
    _vertices.clear();
    _indices.clear();
    
    const veMat4 &camWorldMat = camera->getNodeToWorldMatrix();
    veVec3 right(camWorldMat[0][0], camWorldMat[1][0], camWorldMat[2][0]);
    veVec3 up(camWorldMat[0][1], camWorldMat[1][1], camWorldMat[2][1]);
    veVec3 backward(camWorldMat[0][2], camWorldMat[1][2], camWorldMat[2][2]);
    
    unsigned int idx = 0;
    for (auto particle : particles){
        addQuatParticleToBuffer(idx, particle, right, up, backward);
        ++idx;
    }
    
    if (!_vertices.empty() && !_indices.empty()){
        bool firstUpdate = false;
        if (!_vao) {
            glGenVertexArrays(1, &_vao);
            glGenBuffers(1, &_vbo);
            glGenBuffers(1, &_ibo);
            firstUpdate = true;
        }
        glBindVertexArray(_vao);
        if (!_vertices.empty()){
            glBindBuffer(GL_ARRAY_BUFFER, _vbo);
            glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(_vertices[0]), _vertices.buffer(), GL_STATIC_DRAW);
        }
        
        if (!_indices.empty()){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(_indices[0]), _indices.buffer(), GL_STATIC_DRAW);
        }
        
        if (firstUpdate){
            unsigned int stride = sizeof(GLfloat) * getVertexStride();
            //v
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
            glEnableVertexAttribArray(0);
            
            //col
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(sizeof(GLfloat) * 3));
            glEnableVertexAttribArray(1);
            
            //tc
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(sizeof(GLfloat) * 7));
            glEnableVertexAttribArray(2);
        }
    }
    
}

void veQuatRenderer::addQuatParticleToBuffer(unsigned int idx, veParticle *particle, const veVec3 &right, const veVec3 &up, const veVec3 &backward)
{
    veVec3 halfWidth   = particle->width * 0.5f * right;
    veVec3 halfHeight  = particle->height * 0.5f * up;
    veVec3 leftTop     = particle->position - halfWidth + halfHeight;
    veVec3 leftBottom  = particle->position - halfWidth - halfHeight;
    veVec3 rightTop    = particle->position + halfWidth + halfHeight;
    veVec3 rightBottom = particle->position + halfWidth - halfHeight;
    
    unsigned int iIdx = idx * 4;
    //leftBottom
    _vertices.push_back(leftBottom.x()); _vertices.push_back(leftBottom.y()); _vertices.push_back(leftBottom.z());
    _vertices.push_back(particle->color.x()); _vertices.push_back(particle->color.y()); _vertices.push_back(particle->color.z()); _vertices.push_back(particle->color.w());
    _vertices.push_back(0.0f); _vertices.push_back(0.0f);
    
    //leftTop
    _vertices.push_back(leftTop.x()); _vertices.push_back(leftTop.y()); _vertices.push_back(leftTop.z());
    _vertices.push_back(particle->color.x()); _vertices.push_back(particle->color.y()); _vertices.push_back(particle->color.z()); _vertices.push_back(particle->color.w());
    _vertices.push_back(0.0f); _vertices.push_back(1.0f);
    
    //rightBottom
    _vertices.push_back(rightBottom.x()); _vertices.push_back(rightBottom.y()); _vertices.push_back(rightBottom.z());
    _vertices.push_back(particle->color.x()); _vertices.push_back(particle->color.y()); _vertices.push_back(particle->color.z()); _vertices.push_back(particle->color.w());
    _vertices.push_back(1.0f); _vertices.push_back(0.0f);
    
    //rightTop
    _vertices.push_back(rightTop.x()); _vertices.push_back(rightTop.y()); _vertices.push_back(rightTop.z());
    _vertices.push_back(particle->color.x()); _vertices.push_back(particle->color.y()); _vertices.push_back(particle->color.z()); _vertices.push_back(particle->color.w());
    _vertices.push_back(1.0f); _vertices.push_back(1.0f);
    
    
    _indices.push_back(iIdx); _indices.push_back(iIdx + 3); _indices.push_back(iIdx + 1);
    _indices.push_back(iIdx); _indices.push_back(iIdx + 2); _indices.push_back(iIdx + 3);
    
}

unsigned int veQuatRenderer::getVertexStride() const
{
    return 3 + 4 + 2;
}