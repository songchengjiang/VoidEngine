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
#include "TransformFeedback.h"
#include "MatrixPtr.h"
#include <unordered_map>
#include <thread>

#define BUFFER_OFFSET(idx) (static_cast<char*>(0) + (idx))

veMeshRenderer::veMeshRenderer()
	: _drawUsage(GL_STATIC_DRAW)
    , _mesh(nullptr)
    , _needRefresh(false)
{
    _vaoBuffer = veGLDataBufferManager::instance()->createGLDataBuffer([]() -> GLuint{
        GLuint vao;
        glGenVertexArrays(1, &vao);
        return vao;
    }, [](GLuint vao){
        glDeleteVertexArrays(1, &vao);
    });
    
    _vboBuffer = veGLDataBufferManager::instance()->createGLDataBuffer([]() -> GLuint{
        GLuint vbo;
        glGenBuffers(1, &vbo);
        return vbo;
    }, [](GLuint vbo){
        glDeleteBuffers(1, &vbo);
    });
}

veMeshRenderer::~veMeshRenderer()
{

}

void veMeshRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID)
{
    _mesh = static_cast<veMesh *>(renderableObj);
    //if (mesh->getBoneNum()) _drawUsage = GL_DYNAMIC_DRAW;
    auto vao = _vaoBuffer->getData(contextID);
    if (!vao){
        vao = _vaoBuffer->createData(contextID);
        _needRefresh = true;
    }
    if (_needRefresh) {
        auto vbo = _vboBuffer->getData(contextID);
        if (!vbo){
            vbo = _vboBuffer->createData(contextID);
        }
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        if (!_mesh->getVertexArray()->empty())
            glBufferData(GL_ARRAY_BUFFER, _mesh->getVertexArray()->size() * sizeof((*_mesh->getVertexArray())[0]), _mesh->getVertexArray()->buffer(), _drawUsage);
        
        GLsizei stride = _mesh->getVertexStride();
        GLsizei offset = 0;
        for (unsigned int i = 0; i < _mesh->getVertexAtrributeNum(); ++i) {
            auto attri = _mesh->getVertexAtrribute(i);
            GLboolean normalize = attri.valueType == veMesh::VertexAtrribute::FLOAT ? GL_FALSE : GL_TRUE;
            glVertexAttribPointer(i, attri.valueNum, attri.valueType, normalize, stride, BUFFER_OFFSET(offset));
            glEnableVertexAttribArray(i);
            if (attri.valueType == veMesh::VertexAtrribute::FLOAT) offset += sizeof(GLfloat) * attri.valueNum;
            else if (attri.valueType == veMesh::VertexAtrribute::UINT) offset += sizeof(GLuint) * attri.valueNum;
            else if (attri.valueType == veMesh::VertexAtrribute::USHORT) offset += sizeof(GLushort) * attri.valueNum;
        }
        
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
        
        if (0 < _mesh->getBoneNum()) {
            auto material = _mesh->getMaterial();
            for (size_t tech = 0; tech < material->getTechniqueNum(); ++tech) {
                auto technique = material->getTechnique(tech);
                for (size_t p = 0; p < technique->getPassNum(); ++p) {
                    auto pass = technique->getPass(p);
                    if (!pass->getTransformFeedback()) {
                        veTransformFeedback *tfback = new veTransformFeedback;
                        tfback->addVarying(TF_VARYING_POSITION_KEY.c_str());
                        tfback->addVarying(TF_VARYING_NORMAL_KEY.c_str());
                        pass->setTransformFeedback(tfback);
                    }
                }
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
    
    veRenderCommand rc;
    rc.mask = node->getMask();
    rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix());
    rc.user1 = _mesh;
    rc.camera = camera;
    rc.sceneManager = camera->getSceneManager();
    rc.depthInCamera = (camera->viewMatrix() * rc.worldMatrix->value())[2][3];
    rc.renderer = this;
    rc.contextID = contextID;
    
    auto technique = _mesh->getMaterial()->activeTechnique();
    for (unsigned int i = 0; i < technique->getPassNum(); ++i) {
        auto pass = technique->getPass(i);
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

void veMeshRenderer::draw(veRenderCommand &command)
{
	if (!isNeedRendering())
		return;

	//vePass *renderPass = nullptr;
	//if (veRenderer::CURRENT_RENDER_STAGE == veRenderer::DIRECTIONAL_SHADOW) {
	//	renderPass = command.pass;
	//	auto matAry = static_cast<veMaterialManager *>(command.sceneManager->getManager(veMaterialManager::TYPE()))->findMaterialArray("_SYSTEM_");
	//	command.pass = matAry->getMaterial(0 < mesh->getBoneNum() ? veMaterial::SYSTEM_MATERIAL_DIRECTIONAL_SHADOW_MAP_FOR_ANIM_ENTITY
	//		: veMaterial::SYSTEM_MATERIAL_DIRECTIONAL_SHADOW_MAP_FOR_ENTITY)->activeTechnique()->getPass(0);
	//}
	//else if (veRenderer::CURRENT_RENDER_STAGE == veRenderer::OMNIDIRECTIONAL_SHADOW){
	//	renderPass = command.pass;
	//	auto matAry = static_cast<veMaterialManager *>(command.sceneManager->getManager(veMaterialManager::TYPE()))->findMaterialArray("_SYSTEM_");
	//	command.pass = matAry->getMaterial(0 < mesh->getBoneNum() ? veMaterial::SYSTEM_MATERIAL_OMNIDIRECTIONAL_SHADOW_MAP_FOR_ANIM_ENTITY
	//		: veMaterial::SYSTEM_MATERIAL_OMNIDIRECTIONAL_SHADOW_MAP_FOR_ENTITY)->activeTechnique()->getPass(0);
	//}
	//else if (veRenderer::CURRENT_RENDER_STAGE == veRenderer::LIGHTINGING) {
	//	if (command.pass->blendFunc() != veBlendFunc::DISABLE) return;
	//	renderPass = command.pass;
	//	auto matAry = static_cast<veMaterialManager *>(command.sceneManager->getManager(veMaterialManager::TYPE()))->findMaterialArray("_SYSTEM_");
	//	command.pass = matAry->getMaterial(0 < mesh->getBoneNum() ? veMaterial::SYSTEM_MATERIAL_LIGHTING_PASS_FOR_ANIM_ENTITY
	//		: veMaterial::SYSTEM_MATERIAL_LIGHTING_PASS_FOR_ENTITY)->activeTechnique()->getPass(0);
	//}

	if (!command.pass->apply(command))
		return;
    
	glBindVertexArray(_vaoBuffer->getData(command.contextID));

	auto transformFeedback = command.pass->getTransformFeedback();
	if (transformFeedback) {
		glEnable(GL_RASTERIZER_DISCARD);
		transformFeedback->bind(command.contextID, _mesh->getTransformFeedbackBuffer(command.contextID), _mesh->getTransformFeedbackBufferSize(), GL_POINTS);
		glDrawArrays(GL_POINTS, 0, _mesh->getVertexCount());
		transformFeedback->unBind();
		glDisable(GL_RASTERIZER_DISCARD);
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
		veLog("GL ERROR CODE: 0x%x", ec);
	}
}
