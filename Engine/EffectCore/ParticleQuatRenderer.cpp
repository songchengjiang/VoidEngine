#include "ParticleQuatRenderer.h"
#include "KernelCore/Node.h"
#include "KernelCore/Camera.h"
#include "KernelCore/RenderQueue.h"
#include "ParticleSystem.h"
#include "Particle.h"
#include "Constants.h"

veParticleQuatRenderer::veParticleQuatRenderer()
    : _orientationType(OrientationType::OT_BILLBOARD)
{
//    //0
//    _vertices.push_back(-0.5f); _vertices.push_back(-0.5f); _vertices.push_back(0.0f); //v
//    _vertices.push_back(0.0f); _vertices.push_back(0.0f);                              //tc
//    
//    //1
//    _vertices.push_back(0.5f); _vertices.push_back(-0.5f); _vertices.push_back(0.0f); //v
//    _vertices.push_back(1.0f); _vertices.push_back(0.0f);                              //tc
//    
//    //2
//    _vertices.push_back(-0.5f); _vertices.push_back(0.5f); _vertices.push_back(0.0f); //v
//    _vertices.push_back(0.0f); _vertices.push_back(1.0f);                              //tc
//    
//    //3
//    _vertices.push_back(0.5f); _vertices.push_back(0.5f); _vertices.push_back(0.0f); //v
//    _vertices.push_back(1.0f); _vertices.push_back(1.0f);                              //tc
}

veParticleQuatRenderer::~veParticleQuatRenderer()
{

}

void veParticleQuatRenderer::draw(veRenderCommand &command)
{
    
    if (!command.pass->apply(command))
        return;
    
//    glBindVertexArray(_vaoBuffer->getData(command.contextID));
//    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, _instanceCount);
    
    auto vbo = _vboBuffer->getData(command.contextID);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    unsigned int stride = sizeof(GLfloat) * getVertexStride();
    
    //v
    int vLoc = glGetAttribLocation(command.pass->getProgram(command.contextID), VERTEX_ATTRIBUTE_POSITION_NAME.c_str());
    if (0 <= vLoc) {
        glVertexAttribPointer(vLoc, 3, GL_FLOAT, GL_FALSE, stride, 0);
        glEnableVertexAttribArray(vLoc);
    }
    
    //tc
    int tcLoc = glGetAttribLocation(command.pass->getProgram(command.contextID), VERTEX_ATTRIBUTE_TEXCOORD0_NAME.c_str());
    if (0 <= tcLoc) {
        glVertexAttribPointer(tcLoc, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(sizeof(GLfloat) * 3));
        glEnableVertexAttribArray(tcLoc);
    }
    
    //col
    int coLoc = glGetAttribLocation(command.pass->getProgram(command.contextID), VERTEX_ATTRIBUTE_COLOR0_NAME.c_str());
    if (0 <= coLoc) {
        glVertexAttribPointer(coLoc, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(sizeof(GLfloat) * (3 + 2)));
        glEnableVertexAttribArray(coLoc);
    }
    
    glDrawArrays(GL_TRIANGLES, 0, _vertices.size() / getVertexStride());

    glDisableVertexAttribArray(vLoc);
    glDisableVertexAttribArray(tcLoc);
    glDisableVertexAttribArray(coLoc);
}

void veParticleQuatRenderer::updateInstanceParams(veParticle *particle, const veMat4 &cameraRot, const veMat4 &vp)
{
    veVec3 scale(particle->width, particle->height, 1.0f);
    veMat4 mat = vp * veMat4::translation(particle->position) * cameraRot * veMat4::rotation(particle->orientation) * veMat4::scale(veVec3(particle->width, particle->height, 1.0f));
    //mat.transpose();
    
    veVec3 v0 = mat * veVec3(-0.5f, -0.5f, 0.0f);
    veVec3 v1 = mat * veVec3(0.5f, -0.5f, 0.0f);
    veVec3 v2 = mat * veVec3(-0.5f, 0.5f, 0.0f);
    veVec3 v3 = mat * veVec3(0.5f, 0.5f, 0.0f);
    
    addParticle(v0, v1, v2, v3, particle->color);
}

void veParticleQuatRenderer::addParticle(const veVec3 &v0, const veVec3 &v1, const veVec3 &v2, const veVec3 &v3
                 , const veVec4 &color)
{
    _vertices.push_back(v0.x()); _vertices.push_back(v0.y()); _vertices.push_back(v0.z());
    _vertices.push_back(0.0f); _vertices.push_back(0.0f);
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _vertices.push_back(v1.x()); _vertices.push_back(v1.y()); _vertices.push_back(v1.z());
    _vertices.push_back(1.0f); _vertices.push_back(0.0f);
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _vertices.push_back(v2.x()); _vertices.push_back(v2.y()); _vertices.push_back(v2.z());
    _vertices.push_back(0.0f); _vertices.push_back(1.0f);
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    
    _vertices.push_back(v1.x()); _vertices.push_back(v1.y()); _vertices.push_back(v1.z());
    _vertices.push_back(1.0f); _vertices.push_back(0.0f);
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _vertices.push_back(v3.x()); _vertices.push_back(v3.y()); _vertices.push_back(v3.z());
    _vertices.push_back(1.0f); _vertices.push_back(1.0f);
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _vertices.push_back(v2.x()); _vertices.push_back(v2.y()); _vertices.push_back(v2.z());
    _vertices.push_back(0.0f); _vertices.push_back(1.0f);
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
}

void veParticleQuatRenderer::updateBuffer(veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID)
{
    auto vbo = _vboBuffer->getData(contextID);
    if (!vbo){
        vbo = _vboBuffer->createData(contextID);
        //_needUpdate = true;
    }
//    if (!_vertices.empty() && _needUpdate){
//
//        auto ibo = _iboBuffer->getData(contextID);
//        if (!ibo){
//            ibo = _iboBuffer->createData(contextID);
//        }
//
////        glBindBuffer(GL_ARRAY_BUFFER, vbo);
////        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(_vertices[0]), _vertices.buffer(), GL_STATIC_DRAW);
//
////        unsigned int stride = sizeof(GLfloat) * getVertexStride();
////        //v
////        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
////        glEnableVertexAttribArray(0);
////
////        //tc
////        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(sizeof(GLfloat) * 3));
////        glEnableVertexAttribArray(1);
////
////        glBindBuffer(GL_ARRAY_BUFFER, colorbo);
////        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, 0);
////        glVertexAttribDivisor(2, 1);
////        glEnableVertexAttribArray(2);
////
////        glBindBuffer(GL_ARRAY_BUFFER, mvpbo);
////        for (unsigned short i = 0; i < 4; ++i){
////            glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(veMat4),
////                                  (GLvoid*)(sizeof(GLfloat) * i * 4));
////            glVertexAttribDivisor(3 + i, 1);
////            glEnableVertexAttribArray(3 + i);
////        }
//
//        _needUpdate = false;
//    }

//    _instanceCount = 0;
//    _mvpMats.clear();
//    _colors.clear();
    
    _vertices.clear();
    
    veMat4 cameraRot = veMat4::IDENTITY;
    if (_orientationType == OrientationType::OT_BILLBOARD || _orientationType == OrientationType::OT_DIRECTION){
        cameraRot = camera->getAttachedNode()->getNodeToWorldMatrix();
        cameraRot[0][3] = cameraRot[1][3] = cameraRot[2][3] = 0.0f;
    }
    veVec3 backward(cameraRot[0][2], cameraRot[1][2], cameraRot[2][2]);
    veMat4 viewProjMat = camera->projectionMatrix() * camera->viewMatrix();

    const auto &particles = static_cast<veParticleSystem *>(renderableObj)->getParticles().getActiveDataList();
    static_cast<veParticleSystem *>(renderableObj)->lock();
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
        //++_instanceCount;
    }
    static_cast<veParticleSystem *>(renderableObj)->unLock();
    
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(_vertices[0]), _vertices.buffer(), GL_DYNAMIC_DRAW);
    

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

unsigned int veParticleQuatRenderer::getVertexStride() const
{
    return 3 + 2 + 4;
}
