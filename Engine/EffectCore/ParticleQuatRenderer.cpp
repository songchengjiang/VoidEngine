#include "ParticleQuatRenderer.h"
#include "KernelCore/Node.h"
#include "KernelCore/Camera.h"
#include "KernelCore/RenderQueue.h"
#include "ParticleSystem.h"
#include "Particle.h"

veParticleQuatRenderer::veParticleQuatRenderer()
    : _orientationType(OrientationType::OT_BILLBOARD)
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

veParticleQuatRenderer::~veParticleQuatRenderer()
{

}

void veParticleQuatRenderer::draw(veRenderCommand &command)
{
    if (!isNeedRendering())
        return;
    
    if (!command.pass->apply(command))
        return;
    
    glBindVertexArray(_vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, _instanceCount);
}

void veParticleQuatRenderer::updateInstanceParams(veParticle *particle, const veMat4 &cameraRot, const veMat4 &vp)
{
    veVec3 scale(particle->width, particle->height, 1.0f);
    veMat4 mat = vp * veMat4::translation(particle->position) * cameraRot * veMat4::rotation(particle->orientation) * veMat4::scale(veVec3(particle->width, particle->height, 1.0f));
    mat.transpose();
    _mvpMats.push_back(mat);
    _colors.push_back(particle->color);
}

void veParticleQuatRenderer::updateBuffer(veRenderableObject *renderableObj, veCamera *camera)
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
    veMat4 cameraRot = veMat4::IDENTITY;
    if (_orientationType == OrientationType::OT_BILLBOARD || _orientationType == OrientationType::OT_DIRECTION){
        cameraRot = camera->getNodeToWorldMatrix();
        cameraRot[0][3] = cameraRot[1][3] = cameraRot[2][3] = 0.0f;
    }
    veVec3 backward(cameraRot[0][2], cameraRot[1][2], cameraRot[2][2]);
    veMat4 viewProjMat = camera->projectionMatrix() * camera->viewMatrix();
    
    const auto &particles = static_cast<veParticleSystem *>(renderableObj)->getParticles().getActiveDataList();
    for (auto particle : particles){
        if (_orientationType == OrientationType::OT_DIRECTION){
            veVec3 up = particle->direction;
            up.normalize();
            veVec3 right = up.crossProduct(backward);
            right.normalize();
            veVec3 back = right.crossProduct(up);
            back.normalize();
            cameraRot[0][0] = right.x(); cameraRot[0][1] = up.x(); cameraRot[0][2] = back.x();
            cameraRot[1][0] = right.y(); cameraRot[1][1] = up.y(); cameraRot[1][2] = back.y();
            cameraRot[2][0] = right.z(); cameraRot[2][1] = up.z(); cameraRot[2][2] = back.z();
        }
        updateInstanceParams(particle, cameraRot, viewProjMat);
        ++_instanceCount;
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

unsigned int veParticleQuatRenderer::getVertexStride() const
{
    return 3 + 2;
}