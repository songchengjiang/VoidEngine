#include "MeshRenderer.h"
#include "RenderCommand.h"
#include "Visualiser.h"
#include "RenderableObject.h"
#include "Mesh.h"
#include "Material.h"
#include "Constants.h"
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
				bool isTransparent = pass->blendFunc() != veBlendFunc::DISABLE ? true : false;
				veRenderCommand rc;
				rc.pass = pass;
				rc.attachedNode = node;
				rc.renderableObj = renderableObj;
				rc.camera = camera;
                rc.drawFunc = VE_CALLBACK_1(veMeshRenderer::draw, this);
				if (isTransparent)
					rc.depthInCamera = (camera->viewMatrix() * node->getNodeToWorldMatrix())[2][3];
				pass->visit(rc);
				if (isTransparent)
					veRenderQueue::CURRENT_RENDER_QUEUE->pushCommand(veRenderQueue::RENDER_QUEUE_TRANSPARENT, rc);
				else
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

	for (unsigned int i = 0; i < mesh->getPrimitiveNum(); ++i) {
		auto primitive = mesh->getPrimitive(i);
		glDrawElements(primitive.primitiveType, GLsizei(primitive.indices->size()), GL_UNSIGNED_INT, nullptr);
	}
}