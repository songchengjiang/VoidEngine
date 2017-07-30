#include "MeshRenderer.h"
#include "RenderCommand.h"
#include "Camera.h"
#include "RenderableObject.h"
#include "Mesh.h"
#include "Node.h"
#include "Material.h"
#include "MaterialManager.h"
#include "SceneManager.h"
#include "Constants.h"
#include "MatrixPtr.h"
#include <unordered_map>
#include <thread>

#define BUFFER_OFFSET(idx) (static_cast<char*>(0) + (idx))

veMeshRenderer::veMeshRenderer()
	: _drawUsage(GL_STATIC_DRAW)
    , _mesh(nullptr)
    , _needRefresh(false)
{
    _vboBuffer = veGLDataBufferManager::instance()->createGLDataBuffer([]() -> GLuint{
        GLuint vbo;
        glGenBuffers(1, &vbo);
        return vbo;
    }, [](GLuint vbo){
        glDeleteBuffers(1, &vbo);
    });
    
    _attributesNamesMap[veMesh::VertexAtrribute::VERTEX_ATTRIB_POSITION]     = VERTEX_ATTRIBUTE_POSITION_NAME;
    _attributesNamesMap[veMesh::VertexAtrribute::VERTEX_ATTRIB_NORMAL]       = VERTEX_ATTRIBUTE_NORMAL_NAME;
    _attributesNamesMap[veMesh::VertexAtrribute::VERTEX_ATTRIB_TANGENT]      = VERTEX_ATTRIBUTE_TANGENT_NAME;
    _attributesNamesMap[veMesh::VertexAtrribute::VERTEX_ATTRIB_BITANGENT]    = VERTEX_ATTRIBUTE_BINORMAL_NAME;
    _attributesNamesMap[veMesh::VertexAtrribute::VERTEX_ATTRIB_TEX_COORD0]   = VERTEX_ATTRIBUTE_TEXCOORD0_NAME;
    _attributesNamesMap[veMesh::VertexAtrribute::VERTEX_ATTRIB_TEX_COORD1]   = VERTEX_ATTRIBUTE_TEXCOORD1_NAME;
    _attributesNamesMap[veMesh::VertexAtrribute::VERTEX_ATTRIB_TEX_COORD2]   = VERTEX_ATTRIBUTE_TEXCOORD2_NAME;
    _attributesNamesMap[veMesh::VertexAtrribute::VERTEX_ATTRIB_TEX_COORD3]   = VERTEX_ATTRIBUTE_TEXCOORD3_NAME;
    _attributesNamesMap[veMesh::VertexAtrribute::VERTEX_ATTRIB_COLOR0]       = VERTEX_ATTRIBUTE_COLOR0_NAME;
    _attributesNamesMap[veMesh::VertexAtrribute::VERTEX_ATTRIB_COLOR1]       = VERTEX_ATTRIBUTE_COLOR1_NAME;
    _attributesNamesMap[veMesh::VertexAtrribute::VERTEX_ATTRIB_COLOR2]       = VERTEX_ATTRIBUTE_COLOR2_NAME;
    _attributesNamesMap[veMesh::VertexAtrribute::VERTEX_ATTRIB_COLOR3]       = VERTEX_ATTRIBUTE_COLOR3_NAME;
    _attributesNamesMap[veMesh::VertexAtrribute::VERTEX_ATTRIB_BONE_INDICES] = VERTEX_ATTRIBUTE_BLENDINDICES_NAME;
    _attributesNamesMap[veMesh::VertexAtrribute::VERTEX_ATTRIB_BONE_WEIGHTS] = VERTEX_ATTRIBUTE_BLENDWEIGHTS_NAME;
}

veMeshRenderer::~veMeshRenderer()
{

}

void veMeshRenderer::render(veRenderableObject *renderableObj, veCamera *camera, const veMat4 &worldMatrix, unsigned int contextID, veRenderQueue::RenderQueueType type)
{
    _mesh = static_cast<veMesh *>(renderableObj);
    veRenderCommand rc;
    rc.worldMatrix = new veMat4Ptr(&worldMatrix, 1);
    rc.camera = camera;
    rc.sceneManager = camera->getAttachedNode()->getSceneManager();
    rc.depthInCamera = (camera->viewMatrix() * worldMatrix)[2][3];
    rc.renderer = this;
    rc.contextID = contextID;
    if (0 < _mesh->getBoneNum()) {
        rc.bonesMatrix = new veMat4Ptr(_mesh->getBoneNum());
        veMat4 worldToMesh = worldMatrix;
        worldToMesh.inverse();
        for (size_t i = 0; i < _mesh->getBoneNum(); ++i) {
            const auto &bone = _mesh->getBone(i);
            rc.bonesMatrix->value()[i] = worldToMesh * bone->getBoneNode()->getNodeToWorldMatrix() * bone->getOffsetMat();
        }
    }
    
    
    auto vbo = _vboBuffer->getData(contextID);
    if (!vbo){
        vbo = _vboBuffer->createData(contextID);
        _needRefresh = true;
    }
    if (_needRefresh) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        if (!_mesh->getVertexArray()->empty())
            glBufferData(GL_ARRAY_BUFFER, _mesh->getVertexArray()->size() * sizeof((*_mesh->getVertexArray())[0]), _mesh->getVertexArray()->buffer(), _drawUsage);
        
        _mesh->getVertexStride();
        
        if (_iboBuffers.size() < _mesh->getPrimitiveNum()) {
            _iboBuffers.resize(_mesh->getPrimitiveNum());
            for (auto &iboBuf : _iboBuffers){
                iboBuf = veGLDataBufferManager::instance()->createGLDataBuffer([]() -> GLuint{
                    GLuint ibo;
                    glGenBuffers(1, &ibo);
                    return ibo;
                }, [](GLuint ibo){
                    glDeleteBuffers(1, &ibo);
                });
            }
        }
        
        for (unsigned int i = 0; i < _mesh->getPrimitiveNum(); ++i) {
            auto ibo = _iboBuffers[i]->getData(contextID);
            if (!ibo){
                ibo = _iboBuffers[i]->createData(contextID);
            }
            auto primitive = _mesh->getPrimitive(i);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
            if (!primitive.indices->empty())
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, primitive.indices->size() * sizeof((*primitive.indices)[0]), primitive.indices->buffer(), GL_STATIC_DRAW);
            
        }
        
        _needRefresh = false;
    }
    
    auto technique = _mesh->getMaterial()->activeTechnique();
    for (unsigned int i = 0; i < technique->getPassNum(); ++i) {
        auto pass = technique->getPass(i);
        if (true) {
            rc.pass = pass;
            pass->visit(rc);
            
            if (type == veRenderQueue::AUTO) {
                bool isTransparent = pass->blendFunc() != veBlendFunc::DISABLE ? true : false;
                if (isTransparent)
                    camera->getRenderQueue()->pushCommand(i, veRenderQueue::RENDER_QUEUE_TRANSPARENT, rc);
                else
                    camera->getRenderQueue()->pushCommand(i, veRenderQueue::RENDER_QUEUE_ENTITY, rc);
            }else {
                camera->getRenderQueue()->pushCommand(i, type, rc);
            }
        }
    }
}

void veMeshRenderer::draw(veRenderCommand &command)
{
	if (!command.pass->apply(command))
		return;
    
    glBindBuffer(GL_ARRAY_BUFFER, _vboBuffer->getData(command.contextID));
    GLsizei stride = _mesh->getVertexStride();
    GLsizei offset = 0;
    for (unsigned int i = 0; i < _mesh->getVertexAtrributeNum(); ++i) {
        auto attri = _mesh->getVertexAtrribute(i);
        GLboolean normalize = attri.valueType == veMesh::VertexAtrribute::FLOAT ? GL_FALSE : GL_TRUE;
        int loc = glGetAttribLocation(command.pass->getProgram(command.contextID), _attributesNamesMap[attri.attributeType].c_str());
        if (0 <= loc) {
            glVertexAttribPointer(loc, attri.valueNum, attri.valueType, normalize, stride, BUFFER_OFFSET(offset));
            glEnableVertexAttribArray(loc);
        }
        if (attri.valueType == veMesh::VertexAtrribute::FLOAT) offset += sizeof(GLfloat) * attri.valueNum;
        else if (attri.valueType == veMesh::VertexAtrribute::UINT) offset += sizeof(GLuint) * attri.valueNum;
        else if (attri.valueType == veMesh::VertexAtrribute::USHORT) offset += sizeof(GLushort) * attri.valueNum;
    }

	for (unsigned int i = 0; i < _mesh->getPrimitiveNum(); ++i) {
		auto primitive = _mesh->getPrimitive(i);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iboBuffers[i]->getData(command.contextID));
		glDrawElements(primitive.primitiveType, GLsizei(primitive.indices->size()), GL_UNSIGNED_SHORT, nullptr);
	}
	//if (renderPass) {
	//	command.pass = renderPass;
	//}
	int ec = glGetError();
	if (ec != GL_NO_ERROR) {
		veLog("veMeshRenderer: GL ERROR CODE: 0x%x\n", ec);
	}

    for (unsigned int i = 0; i < _mesh->getVertexAtrributeNum(); ++i) {
        auto attri = _mesh->getVertexAtrribute(i);
        int loc = glGetAttribLocation(command.pass->getProgram(command.contextID), _attributesNamesMap[attri.attributeType].c_str());
        if (0 <= loc) {
            glDisableVertexAttribArray(loc);
        }
    }
}
