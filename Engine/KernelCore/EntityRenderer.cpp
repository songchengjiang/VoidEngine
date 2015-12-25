#include "EntityRenderer.h"
#include "RenderCommand.h"
#include "Camera.h"
#include "RenderableObject.h"
#include "Entity.h"
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

veEntityRenderer::veEntityRenderer()
	: _drawUsage(GL_STATIC_DRAW)
{
}

veEntityRenderer::~veEntityRenderer()
{

}

void veEntityRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera)
{
	if (!isNeedRendering())
		return;
	if (true) {

		veEntity *entity = static_cast<veEntity *>(renderableObj);

		if (_meshBuffersList.size() < entity->getMeshCount()) {
			_meshBuffersList.resize(entity->getMeshCount());
		}
		veMat4 nTow = node->getNodeToWorldMatrix();
		for (size_t i = 0; i < entity->getMeshCount(); ++i) {
			veMesh *mesh = entity->getMesh(i);
			MeshBuffers &buffers = _meshBuffersList[i];
			GLuint &vao = buffers.vao;
			GLuint &vbo = buffers.vbo;
			std::vector<GLuint> &ibos = buffers.ibos;
			//if (mesh->getBoneNum()) _drawUsage = GL_DYNAMIC_DRAW;
			if (mesh->needRefresh()) {
				if (!vao) {
					glGenVertexArrays(1, &vao);
					glGenBuffers(1, &vbo);
				}
				glBindVertexArray(vao);
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				if (!mesh->getVertexArray()->empty())
					glBufferData(GL_ARRAY_BUFFER, mesh->getVertexArray()->size() * sizeof((*mesh->getVertexArray())[0]), mesh->getVertexArray()->buffer(), _drawUsage);

				GLsizei stride = mesh->getVertexStride();
				GLsizei offset = 0;
				for (unsigned int i = 0; i < mesh->getVertexAtrributeNum(); ++i) {
					auto attri = mesh->getVertexAtrribute(i);
					GLboolean normalize = attri.valueType == veMesh::VertexAtrribute::FLOAT ? GL_FALSE : GL_TRUE;
					glVertexAttribPointer(i, attri.valueNum, attri.valueType, normalize, stride, BUFFER_OFFSET(offset));
					glEnableVertexAttribArray(i);
					if (attri.valueType == veMesh::VertexAtrribute::FLOAT) offset += sizeof(GLfloat) * attri.valueNum;
					else if (attri.valueType == veMesh::VertexAtrribute::UINT) offset += sizeof(GLuint) * attri.valueNum;
					else if (attri.valueType == veMesh::VertexAtrribute::USHORT) offset += sizeof(GLushort) * attri.valueNum;
				}

				if (ibos.size() < mesh->getPrimitiveNum()) {
					ibos.resize(mesh->getPrimitiveNum(), 0);
				}

				if (0 < mesh->getBoneNum()) {
					auto material = mesh->getMaterial();
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

				for (unsigned int i = 0; i < mesh->getPrimitiveNum(); ++i) {
					if (!ibos[i]) {
						glGenBuffers(1, &ibos[i]);
					}
					auto primitive = mesh->getPrimitive(i);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibos[i]);
					if (!primitive.indices->empty())
						glBufferData(GL_ELEMENT_ARRAY_BUFFER, primitive.indices->size() * sizeof((*primitive.indices)[0]), primitive.indices->buffer(), GL_STATIC_DRAW);

				}

				mesh->needRefresh() = false;
			}

			veMat4 meshRootMat = nTow * mesh->getAttachedNode()->toMeshNodeRootMatrix();
			veRenderCommand rc;
			rc.mask = node->getMask();
			rc.worldMatrix = new veMat4Ptr(meshRootMat);
			rc.renderableObj = renderableObj;
			rc.user1 = node;
			rc.user2 = mesh;
			rc.user3 = &buffers;
			rc.camera = camera;
			rc.sceneManager = camera->getSceneManager();
			rc.depthInCamera = (camera->viewMatrix() * rc.worldMatrix->value())[2][3];
			rc.renderer = this;

			auto technique = mesh->getMaterial()->activeTechnique();
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
	}
}

void veEntityRenderer::draw(veRenderCommand &command)
{
	MeshBuffers *bufs = static_cast<MeshBuffers *>(command.user3);
	veMesh *mesh = static_cast<veMesh *>(command.user2);

	vePass *renderPass = nullptr;
	if (veRenderer::CURRENT_RENDER_STAGE == veRenderer::DIRECTIONAL_SHADOW) {
		renderPass = command.pass;
		auto matAry = static_cast<veMaterialManager *>(command.sceneManager->getManager(veMaterialManager::TYPE()))->findMaterialArray("_SYSTEM_");
		command.pass = matAry->getMaterial(0 < mesh->getBoneNum() ? veMaterial::SYSTEM_MATERIAL_DIRECTIONAL_SHADOW_MAP_FOR_ANIM_ENTITY
			: veMaterial::SYSTEM_MATERIAL_DIRECTIONAL_SHADOW_MAP_FOR_ENTITY)->activeTechnique()->getPass(0);
	}
	else if (veRenderer::CURRENT_RENDER_STAGE == veRenderer::OMNIDIRECTIONAL_SHADOW){
		renderPass = command.pass;
		auto matAry = static_cast<veMaterialManager *>(command.sceneManager->getManager(veMaterialManager::TYPE()))->findMaterialArray("_SYSTEM_");
		command.pass = matAry->getMaterial(0 < mesh->getBoneNum() ? veMaterial::SYSTEM_MATERIAL_OMNIDIRECTIONAL_SHADOW_MAP_FOR_ANIM_ENTITY
			: veMaterial::SYSTEM_MATERIAL_OMNIDIRECTIONAL_SHADOW_MAP_FOR_ENTITY)->activeTechnique()->getPass(0);
	}
	else if (veRenderer::CURRENT_RENDER_STAGE == veRenderer::LIGHTINGING) {
		if (command.pass->blendFunc() != veBlendFunc::DISABLE) return;
		renderPass = command.pass;
		auto matAry = static_cast<veMaterialManager *>(command.sceneManager->getManager(veMaterialManager::TYPE()))->findMaterialArray("_SYSTEM_");
		command.pass = matAry->getMaterial(0 < mesh->getBoneNum() ? veMaterial::SYSTEM_MATERIAL_LIGHTING_PASS_FOR_ANIM_ENTITY
			: veMaterial::SYSTEM_MATERIAL_LIGHTING_PASS_FOR_ENTITY)->activeTechnique()->getPass(0);
	}

	if (!command.pass->apply(command))
		return;
	glBindVertexArray(bufs->vao);

	auto transformFeedback = command.pass->getTransformFeedback();
	if (transformFeedback) {
		glEnable(GL_RASTERIZER_DISCARD);
		transformFeedback->bind(mesh->getTransformFeedbackBuffer(), mesh->getTransformFeedbackBufferSize(), GL_POINTS);
		glDrawArrays(GL_POINTS, 0, mesh->getVertexCount());
		transformFeedback->unBind();
		glDisable(GL_RASTERIZER_DISCARD);
	}

	for (unsigned int i = 0; i < mesh->getPrimitiveNum(); ++i) {
		auto primitive = mesh->getPrimitive(i);
		glDrawElements(primitive.primitiveType, GLsizei(primitive.indices->size()), GL_UNSIGNED_SHORT, nullptr);
	}

	if (renderPass) {
		command.pass = renderPass;
	}
	int ec = glGetError();
	if (ec != GL_NO_ERROR) {
		veLog("GL ERROR CODE: 0x%x", ec);
	}
}
