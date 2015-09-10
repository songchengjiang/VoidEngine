#include "MeshRenderer.h"
#include "RenderCommand.h"
#include "Visualiser.h"
#include "RenderableObject.h"
#include "Mesh.h"
#include "Material.h"
#include <unordered_map>

veMeshRenderer::veMeshRenderer()
	: _vao(0)
	, _vbo(0)
	, _drawUsage(GL_STATIC_DRAW)
{
}

veMeshRenderer::~veMeshRenderer()
{

}

void veMeshRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera)
{
	if (_technique) {
		veMesh *mesh = static_cast<veMesh *>(renderableObj);
		if (mesh->getBoneNum()) _drawUsage = GL_DYNAMIC_DRAW;
		for (unsigned int i = 0; i < _technique->getPassNum(); ++i) {
			auto pass = _technique->getPass(i);
			if (camera->getMask() & pass->drawMask()) {
				veRenderCommand rc;
				rc.pass = pass;
				rc.attachedNode = node;
				rc.renderableObj = renderableObj;
				rc.camera = camera;
				rc.renderer = this;
				pass->visit(rc);
				veRenderQueue::CURRENT_RENDER_QUEUE->pushCommand(veRenderQueue::RENDER_QUEUE_ENTITY, rc);
			}
		}
	}
}

void veMeshRenderer::draw(const veRenderCommand &command)
{
	//glDrawPixels(command.pass->getTexture(0)->getImage()->width(), command.pass->getTexture(0)->getImage()->height(), command.pass->getTexture(0)->getImage()->pixelFormat(), command.pass->getTexture(0)->getImage()->dataType(), command.pass->getTexture(0)->getImage()->data());
	command.pass->apply(command);
	auto mesh = static_cast<veMesh *>(command.renderableObj);

	if (!_vao) {
		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vbo);
	}

	glBindVertexArray(_vao);
	if (mesh->needRefresh()) {
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
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

		if (_ibos.size() < mesh->getPrimitiveNum()) {
			_ibos.resize(mesh->getPrimitiveNum(), 0);
		}

		for (unsigned int i = 0; i < mesh->getPrimitiveNum(); ++i) {
			if (!_ibos[i]) {
				glGenBuffers(1, &_ibos[i]);
			}
			auto primitive = mesh->getPrimitive(i);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibos[i]);
			if (!primitive.indices->empty())
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, primitive.indices->size() * sizeof((*primitive.indices)[0]), primitive.indices->buffer(), GL_STATIC_DRAW);

		}

		mesh->needRefresh() = false;
	}

	if (mesh->getBoneNum())
		updateBones(command.attachedNode, mesh);

	for (unsigned int i = 0; i < mesh->getPrimitiveNum(); ++i) {
		auto primitive = mesh->getPrimitive(i);
		glDrawElements(primitive.primitiveType, primitive.indices->size(), GL_UNSIGNED_INT, nullptr);
	}
}

void veMeshRenderer::updateBones(veNode *node, veMesh *mesh)
{
	unsigned char *ary = (unsigned char *)mesh->getVertexArray()->buffer();
	unsigned char *buf = (unsigned char *)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);

	//std::unordered_map<unsigned int, veMat4> vertexBoneMats;
	//veMat4 worldToMesh = node->getWorldToNodeMatrix();
	//for (unsigned int i = 0; i < mesh->getBoneNum(); ++i) {
	//	const auto &bone = mesh->getBone(i);
	//	veMat4 boneToWorld = bone->getBoneNode()->getNodeToWorldMatrix();
	//	veMat4 worldmat = worldToMesh * boneToWorld * bone->getOffsetMat();
	//	for (auto &iter : bone->getWeights()) {
	//		//veReal *vertex = (veReal *)(buf + iter.first * mesh->getVertexStride());
	//		//memset(vertex, 0, 3 * sizeof(veReal));
	//		auto m = vertexBoneMats.find(iter.first);
	//		if (m == vertexBoneMats.end()) {
	//			vertexBoneMats[iter.first] = worldmat * iter.second;
	//		}
	//		else {
	//			m->second = m->second + worldmat * iter.second;
	//		}
	//	}
	//}

	////for (unsigned int i = 0; i < mesh->getBoneNum(); ++i) {
	////	const auto &bone = mesh->getBone(i);
	////	for (auto &iter : bone->getWeights()) {
	////		veVec3 *vertex = (veVec3 *)(buf + iter.first * mesh->getVertexStride());
	////		//veVec3 *normal = (vertex + 1);
	////		//veVec3 *originV = (veVec3 *)(ary + iter.first * mesh->getVertexStride());
	////		//veVec3 *originN = (originV + 1);
	////		//*vertex = worldmat * (*originV) * iter.second;
	////	}
	////}

	//for (auto &iter : vertexBoneMats) {
	//	veVec3 *vertex = (veVec3 *)(buf + iter.first * mesh->getVertexStride());
	//	veVec3 *originV = (veVec3 *)(ary + iter.first * mesh->getVertexStride());
	//	*vertex = iter.second * (*originV);
	//}
	glUnmapBuffer(GL_ARRAY_BUFFER);
}
