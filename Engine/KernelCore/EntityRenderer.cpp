#include "EntityRenderer.h"
#include "RenderCommand.h"
#include "Visualiser.h"
#include "RenderableObject.h"
#include "Entity.h"
#include "Node.h"
#include "Material.h"
#include "Constants.h"
#include "TransformFeedback.h"
#include <unordered_map>

veEntityRenderer::veEntityRenderer()
	: _drawUsage(GL_STATIC_DRAW)
{
}

veEntityRenderer::~veEntityRenderer()
{

}

void veEntityRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera)
{
	if (true) {
		veEntity *entity = static_cast<veEntity *>(renderableObj);
		if (_meshBuffersList.size() < entity->getMeshCount()) {
			_meshBuffersList.resize(entity->getMeshCount());
		}
		veMat4 nTow = node->getNodeToWorldMatrix();
		for (size_t i = 0; i < entity->getMeshCount(); ++i) {
			veMesh *mesh = entity->getMesh(i);
			MeshBuffers &buffers = _meshBuffersList[i];
			buffers.node = node;
			buffers.mesh = mesh;
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
					glVertexAttribPointer(i, attri.valueNum, attri.valueType, normalize, stride, (GLvoid*)offset);
					glEnableVertexAttribArray(i);
					if (attri.valueType == veMesh::VertexAtrribute::FLOAT) offset += sizeof(GLfloat) * attri.valueNum;
					else if (attri.valueType == veMesh::VertexAtrribute::UINT) offset += sizeof(GLuint) * attri.valueNum;
					else if (attri.valueType == veMesh::VertexAtrribute::USHORT) offset += sizeof(GLushort) * attri.valueNum;
				}

				if (ibos.size() < mesh->getPrimitiveNum()) {
					ibos.resize(mesh->getPrimitiveNum(), 0);
				}

				if (0 < mesh->getBoneNum()) {
					GLsizei tfBufferSize = 0;
					for (unsigned int i = 0; i < mesh->getVertexAtrributeNum(); ++i) {
						auto attri = mesh->getVertexAtrribute(i);
						if (attri.attributeType == veMesh::VertexAtrribute::VERTEX_ATTRIB_POSITION
					     || attri.attributeType == veMesh::VertexAtrribute::VERTEX_ATTRIB_NORMAL) {
							if (attri.valueType == veMesh::VertexAtrribute::FLOAT) tfBufferSize += sizeof(GLfloat) * attri.valueNum;
							else if (attri.valueType == veMesh::VertexAtrribute::UINT) tfBufferSize += sizeof(GLuint) * attri.valueNum;
							else if (attri.valueType == veMesh::VertexAtrribute::USHORT) tfBufferSize += sizeof(GLushort) * attri.valueNum;
						}
					}
					unsigned int vertexCount = (mesh->getVertexArray()->size() * sizeof(GLfloat)) / stride;
					tfBufferSize *= vertexCount;
					auto material = mesh->getMaterial();
					for (size_t tech = 0; tech < material->getTechniqueNum(); ++tech) {
						auto technique = material->getTechnique(tech);
						for (size_t p = 0; p < technique->getPassNum(); ++p) {
							auto pass = technique->getPass(p);
							if (!pass->getTransformFeedback()) {
								veTransformFeedback *tfback = new veTransformFeedback;
								tfback->addVarying(TF_VARYING_POSITION_KEY.c_str());
								tfback->addVarying(TF_VARYING_NORMAL_KEY.c_str());
								tfback->setBufferSize(GL_ARRAY_BUFFER, tfBufferSize);
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

			auto technique = mesh->getMaterial()->activeTechnique();
			veMat4 meshRootMat =  mesh->getAttachedNode()->toMeshNodeRootMatrix();
			for (unsigned int i = 0; i < technique->getPassNum(); ++i) {
				auto pass = technique->getPass(i);
				if (camera->getMask() & pass->drawMask()) {
					bool isTransparent = pass->blendFunc() != veBlendFunc::DISABLE ? true : false;
					veRenderCommand rc;
					rc.pass = pass;
					rc.worldMatrix = nTow * meshRootMat;
					//rc.attachedNode = node;
					rc.renderableObj = renderableObj;
					rc.userData = &buffers;
					rc.camera = camera;
					rc.drawFunc = VE_CALLBACK_1(veEntityRenderer::draw, this);
					rc.depthInCamera = (camera->viewMatrix() * rc.worldMatrix)[2][3];
					pass->visit(rc);
					if (isTransparent)
						veRenderQueue::CURRENT_RENDER_QUEUE->pushCommand(veRenderQueue::RENDER_QUEUE_TRANSPARENT, rc);
					else
						veRenderQueue::CURRENT_RENDER_QUEUE->pushCommand(veRenderQueue::RENDER_QUEUE_ENTITY, rc);
				}
			}
		}
	}
}

void veEntityRenderer::draw(const veRenderCommand &command)
{
	command.pass->apply(command);
	MeshBuffers *meshBuffers = static_cast<MeshBuffers *>(command.userData);
	auto mesh = meshBuffers->mesh;
	glBindVertexArray(meshBuffers->vao);

	auto transformFeedback = command.pass->getTransformFeedback();
	for (unsigned int i = 0; i < mesh->getPrimitiveNum(); ++i) {
		auto primitive = mesh->getPrimitive(i);
		if (transformFeedback) {
			transformFeedback->bind(primitive.primitiveType);
		}
		glDrawElements(primitive.primitiveType, GLsizei(primitive.indices->size()), GL_UNSIGNED_INT, nullptr);
		if (transformFeedback) {
			transformFeedback->unBind();
		}
	}

	//GLfloat *vertex = (GLfloat *)transformFeedback->mapingBuffer(GL_READ_ONLY);
	//unsigned int vertexSize = transformFeedback->getBufferSize() / sizeof(GLfloat);
	//for (unsigned int i = 0; i < vertexSize; ++i) {
	//	GLfloat val = vertex[i];
	//	std::cout << val <<std::endl;
	//}
	//transformFeedback->unMapingBuffer();
}