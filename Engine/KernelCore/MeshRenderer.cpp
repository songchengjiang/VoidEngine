#include "MeshRenderer.h"
#include "RenderCommand.h"
#include "Visualiser.h"
#include "RenderableObject.h"
#include "Mesh.h"
#include "Material.h"

veMeshRenderer::veMeshRenderer()
	: _vao(0)
{
}

veMeshRenderer::~veMeshRenderer()
{

}

void veMeshRenderer::visit(veNode *node, veRenderableObject *renderableObj, veVisualiser *vs)
{
	veRenderer::visit(node, renderableObj, vs);
	if (_technique){
		veMesh *mesh = static_cast<veMesh *>(renderableObj);
		for (unsigned int i = 0; i < _technique->getPassNum(); ++i){
			veRenderCommand rc;
			rc.pass = _technique->getPass(i);
			rc.attachedNode = node;
			rc.renderableObj = renderableObj;
			rc.visualizer = vs;
			rc.renderer = this;
			vs->getRenderQueue().pushCommand(veRenderQueue::RENDER_QUEUE_ENTITY, rc);
		}
	}
}

void veMeshRenderer::render(const veRenderCommand &command)
{
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
			glBufferData(GL_ARRAY_BUFFER, mesh->getVertexArray()->size() * sizeof((*mesh->getVertexArray())[0]), mesh->getVertexArray()->buffer(), GL_STATIC_DRAW);

		GLsizei stride = 0;
		for (unsigned int i = 0; i < mesh->getVertexAtrributeNum(); ++i){
			auto attri = mesh->getVertexAtrribute(i);
			if (attri.valueType == veMesh::VertexAtrribute::FLOAT) stride += sizeof(GLfloat) * attri.valueNum;
			else if (attri.valueType == veMesh::VertexAtrribute::UINT) stride += sizeof(GLuint) * attri.valueNum;
			else if (attri.valueType == veMesh::VertexAtrribute::USHORT) stride += sizeof(GLushort) * attri.valueNum;
		}

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

	for (unsigned int i = 0; i < mesh->getPrimitiveNum(); ++i) {
		auto primitive = mesh->getPrimitive(i);
		glDrawElements(primitive.primitiveType, primitive.indices->size(), GL_UNSIGNED_INT, nullptr);
	}
}
