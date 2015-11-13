#include "BoxRenderer.h"
#include "RenderQueue.h"
#include "Node.h"
#include "Camera.h"

veBoxRenderer::veBoxRenderer()
	: _vao(0)
	, _vbo(0)
	, _ibo(0)
	, _needRefresh(true)
{
	//NEGATIVE_X
	_vertices.push_back(-1.0f); _vertices.push_back(-1.0f); _vertices.push_back(-1.0f); //v
	_vertices.push_back(-1.0f); _vertices.push_back(0.0f); _vertices.push_back(0.0f);   //n
	_vertices.push_back(0.0f); _vertices.push_back(0.0f);                              //tc

	_vertices.push_back(-1.0f); _vertices.push_back(1.0f); _vertices.push_back(-1.0f); //v
	_vertices.push_back(-1.0f); _vertices.push_back(0.0f); _vertices.push_back(0.0f);   //n
	_vertices.push_back(0.0f); _vertices.push_back(1.0f);                              //tc

	_vertices.push_back(-1.0f); _vertices.push_back(-1.0f); _vertices.push_back(1.0f); //v
	_vertices.push_back(-1.0f); _vertices.push_back(0.0f); _vertices.push_back(0.0f);   //n
	_vertices.push_back(1.0f); _vertices.push_back(0.0f);                              //tc

	_vertices.push_back(-1.0f); _vertices.push_back(1.0f); _vertices.push_back(1.0f); //v
	_vertices.push_back(-1.0f); _vertices.push_back(0.0f); _vertices.push_back(0.0f);   //n
	_vertices.push_back(1.0f); _vertices.push_back(1.0f);                              //tc

	//POSITIVE_Z
	_vertices.push_back(-1.0f); _vertices.push_back(-1.0f); _vertices.push_back(1.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(0.0f); _vertices.push_back(1.0f);   //n
	_vertices.push_back(0.0f); _vertices.push_back(0.0f);                              //tc

	_vertices.push_back(-1.0f); _vertices.push_back(1.0f); _vertices.push_back(1.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(0.0f); _vertices.push_back(1.0f);   //n
	_vertices.push_back(0.0f); _vertices.push_back(1.0f);                                 //tc

	_vertices.push_back(1.0f); _vertices.push_back(-1.0f); _vertices.push_back(1.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(0.0f); _vertices.push_back(1.0f);   //n
	_vertices.push_back(1.0f); _vertices.push_back(0.0f);                              //tc

	_vertices.push_back(1.0f); _vertices.push_back(1.0f); _vertices.push_back(1.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(0.0f); _vertices.push_back(1.0f);   //n
	_vertices.push_back(1.0f); _vertices.push_back(1.0f);                              //tc

	//POSITIVE_X
	_vertices.push_back(1.0f); _vertices.push_back(-1.0f); _vertices.push_back(1.0f); //v
	_vertices.push_back(1.0f); _vertices.push_back(0.0f); _vertices.push_back(1.0f);   //n
	_vertices.push_back(0.0f); _vertices.push_back(0.0f);                              //tc

	_vertices.push_back(1.0f); _vertices.push_back(1.0f); _vertices.push_back(1.0f); //v
	_vertices.push_back(1.0f); _vertices.push_back(0.0f); _vertices.push_back(1.0f);   //n
	_vertices.push_back(0.0f); _vertices.push_back(1.0f);                                 //tc

	_vertices.push_back(1.0f); _vertices.push_back(-1.0f); _vertices.push_back(-1.0f); //v
	_vertices.push_back(1.0f); _vertices.push_back(0.0f); _vertices.push_back(1.0f);   //n
	_vertices.push_back(1.0f); _vertices.push_back(0.0f);                              //tc

	_vertices.push_back(1.0f); _vertices.push_back(1.0f); _vertices.push_back(-1.0f); //v
	_vertices.push_back(1.0f); _vertices.push_back(0.0f); _vertices.push_back(1.0f);   //n
	_vertices.push_back(1.0f); _vertices.push_back(1.0f);                              //tc

	//NEGATIVE_Z
	_vertices.push_back(1.0f); _vertices.push_back(-1.0f); _vertices.push_back(-1.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(0.0f); _vertices.push_back(-1.0f);   //n
	_vertices.push_back(0.0f); _vertices.push_back(0.0f);                              //tc

	_vertices.push_back(1.0f); _vertices.push_back(1.0f); _vertices.push_back(-1.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(0.0f); _vertices.push_back(-1.0f);   //n
	_vertices.push_back(0.0f); _vertices.push_back(1.0f);                                 //tc

	_vertices.push_back(-1.0f); _vertices.push_back(-1.0f); _vertices.push_back(-1.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(0.0f); _vertices.push_back(-1.0f);   //n
	_vertices.push_back(1.0f); _vertices.push_back(0.0f);                              //tc

	_vertices.push_back(-1.0f); _vertices.push_back(1.0f); _vertices.push_back(-1.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(0.0f); _vertices.push_back(-1.0f);   //n
	_vertices.push_back(1.0f); _vertices.push_back(1.0f);                              //tc

	//POSITIVE_Y
	_vertices.push_back(-1.0f); _vertices.push_back(1.0f); _vertices.push_back(1.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(1.0f); _vertices.push_back(0.0f);   //n
	_vertices.push_back(0.0f); _vertices.push_back(0.0f);                              //tc

	_vertices.push_back(-1.0f); _vertices.push_back(1.0f); _vertices.push_back(-1.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(1.0f); _vertices.push_back(0.0f);   //n
	_vertices.push_back(0.0f); _vertices.push_back(1.0f);                                 //tc

	_vertices.push_back(1.0f); _vertices.push_back(1.0f); _vertices.push_back(1.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(1.0f); _vertices.push_back(0.0f);   //n
	_vertices.push_back(1.0f); _vertices.push_back(0.0f);                              //tc

	_vertices.push_back(1.0f); _vertices.push_back(1.0f); _vertices.push_back(-1.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(1.0f); _vertices.push_back(0.0f);   //n
	_vertices.push_back(1.0f); _vertices.push_back(1.0f);                              //tc

	//NEGATIVE_Y
	_vertices.push_back(-1.0f); _vertices.push_back(-1.0f); _vertices.push_back(-1.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(-1.0f); _vertices.push_back(0.0f);   //n
	_vertices.push_back(0.0f); _vertices.push_back(0.0f);                              //tc

	_vertices.push_back(-1.0f); _vertices.push_back(-1.0f); _vertices.push_back(1.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(-1.0f); _vertices.push_back(0.0f);   //n
	_vertices.push_back(0.0f); _vertices.push_back(1.0f);                                 //tc

	_vertices.push_back(1.0f); _vertices.push_back(-1.0f); _vertices.push_back(-1.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(-1.0f); _vertices.push_back(0.0f);   //n
	_vertices.push_back(1.0f); _vertices.push_back(0.0f);                              //tc

	_vertices.push_back(1.0f); _vertices.push_back(-1.0f); _vertices.push_back(1.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(-1.0f); _vertices.push_back(0.0f);   //n
	_vertices.push_back(1.0f); _vertices.push_back(1.0f);                              //tc

	for (unsigned int face = 0; face < 6; ++face) {
		unsigned int idx = 4 * face;
		_indices.push_back(idx + 0); _indices.push_back(idx + 2); _indices.push_back(idx + 1);
		_indices.push_back(idx + 1); _indices.push_back(idx + 2); _indices.push_back(idx + 3);
	}
}

veBoxRenderer::~veBoxRenderer()
{

}

void veBoxRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera)
{
	updateBuffer();
	auto materials = renderableObj->getMaterialArray();
	for (unsigned int mat = 0; mat < materials->getMaterialNum(); ++mat) {
		auto material = materials->getMaterial(mat);
		for (unsigned int i = 0; i < material->activeTechnique()->getPassNum(); ++i) {
			auto pass = material->activeTechnique()->getPass(i);
			if (camera->getMask() & pass->drawMask()) {
				bool isTransparent = pass->blendFunc() != veBlendFunc::DISABLE ? true : false;
				veRenderCommand rc;
				rc.pass = pass;
				rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix());
				//rc.attachedNode = node;
				rc.renderableObj = renderableObj;
				rc.camera = camera;
				rc.depthInCamera = (camera->viewMatrix() * rc.worldMatrix->value())[2][3];
				rc.drawFunc = VE_CALLBACK_1(veBoxRenderer::draw, this);
				pass->visit(rc);
				if (isTransparent)
					camera->getRenderQueue()->pushCommand(veRenderQueue::RENDER_QUEUE_TRANSPARENT, rc);
				else
					camera->getRenderQueue()->pushCommand(veRenderQueue::RENDER_QUEUE_ENTITY, rc);
			}
		}
	}
}

void veBoxRenderer::draw(const veRenderCommand &command)
{
	command.pass->apply(command);
	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, GLsizei(_indices.size()), GL_UNSIGNED_SHORT, nullptr);
}

void veBoxRenderer::updateBuffer()
{
	if (_needRefresh) {
		if (!_vao) {
			glGenVertexArrays(1, &_vao);
			glGenBuffers(1, &_vbo);
		}
		glBindVertexArray(_vao);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		if (!_vertices.empty())
			glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(_vertices[0]), _vertices.buffer(), GL_STATIC_DRAW);

		unsigned int stride = sizeof(GLfloat) * 8;
		//v
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
		glEnableVertexAttribArray(0);

		//n
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(sizeof(GLfloat) * 3));
		glEnableVertexAttribArray(1);

		//tc
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(sizeof(GLfloat) * 6));
		glEnableVertexAttribArray(2);

		if (!_ibo) {
			glGenBuffers(1, &_ibo);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(_indices[0]), _indices.buffer(), GL_STATIC_DRAW);

		_needRefresh = false;
	}
}
