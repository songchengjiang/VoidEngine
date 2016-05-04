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
    
    if (!_vao) {
        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);
        glGenBuffers(1, &_ibo);
    }
    glBindVertexArray(_vao);
    
    unsigned int vertexNumPerParticle = 4 * getVertexStride();
    if (_vertices.size() < particles.size() * vertexNumPerParticle){
        _vertices.resize(particles.size() * vertexNumPerParticle);
    }
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
    
    
    if (!_vertices.empty()){
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(_vertices[0]), _vertices.buffer(), GL_STATIC_DRAW);
    }
    
    if (!_indices.empty()){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(_indices[0]), _indices.buffer(), GL_STATIC_DRAW);
    }
    
    unsigned int stride = sizeof(GLfloat) * getVertexStride();
    //v
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    
    //col
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(sizeof(GLfloat) * 3));
    glEnableVertexAttribArray(1);
    
    //tc
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(sizeof(GLfloat) * 6));
    glEnableVertexAttribArray(2);
}

void veQuatRenderer::addQuatParticleToBuffer(unsigned int idx, veParticle *particle, const veVec3 &right, const veVec3 &up, const veVec3 &backward)
{
    veVec3 halfWidth = particle->width * 0.5f * right;
    veVec3 halfHeight = particle->height * 0.5f * up;
    veVec3 leftTop = particle->position - halfWidth + halfHeight;
    veVec3 leftBottom = particle->position - halfWidth - halfHeight;
    veVec3 rightTop = particle->position + halfWidth + halfHeight;
    veVec3 rightBottom = particle->position + halfWidth - halfHeight;
    
    unsigned int vIdx = idx * 4 * getVertexStride(), iIdx = idx * 4;
    //leftBottom
    _vertices[vIdx++] = leftBottom.x(); _vertices[vIdx++] = leftBottom.y(); _vertices[vIdx++] = leftBottom.z();
    _vertices[vIdx++] = particle->color.x(); _vertices[vIdx++] = particle->color.y(); _vertices[vIdx++] = particle->color.z();
    _vertices[vIdx++] = 0.0f; _vertices[vIdx++] = 0.0f;
    
    //leftTop
    _vertices[vIdx++] = leftTop.x(); _vertices[vIdx++] = leftTop.y(); _vertices[vIdx++] = leftTop.z();
    _vertices[vIdx++] = particle->color.x(); _vertices[vIdx++] = particle->color.y(); _vertices[vIdx++] = particle->color.z();
    _vertices[vIdx++] = 0.0f; _vertices[vIdx++] = 1.0f;
    
    //rightBottom
    _vertices[vIdx++] = rightBottom.x(); _vertices[vIdx++] = rightBottom.y(); _vertices[vIdx++] = rightBottom.z();
    _vertices[vIdx++] = particle->color.x(); _vertices[vIdx++] = particle->color.y(); _vertices[vIdx++] = particle->color.z();
    _vertices[vIdx++] = 1.0f; _vertices[vIdx++] = 0.0f;
    
    //rightTop
    _vertices[vIdx++] = rightTop.x(); _vertices[vIdx++] = rightTop.y(); _vertices[vIdx++] = rightTop.z();
    _vertices[vIdx++] = particle->color.x(); _vertices[vIdx++] = particle->color.y(); _vertices[vIdx++] = particle->color.z();
    _vertices[vIdx++] = 1.0f; _vertices[vIdx++] = 1.0f;
    
    
    _indices.push_back(iIdx); _indices.push_back(iIdx + 3); _indices.push_back(iIdx + 1);
    _indices.push_back(iIdx); _indices.push_back(iIdx + 2); _indices.push_back(iIdx + 3);
    
}

unsigned int veQuatRenderer::getVertexStride() const
{
    return 3 + 3 + 2;
}