#include "SurfaceRenderer.h"
#include "RenderQueue.h"
#include "Visualiser.h"
#include "Node.h"
#include "Overlay.h"
#include "MatrixPtr.h"

veSurfaceRenderer::veSurfaceRenderer()
	: _vao(0)
	, _vbo(0)
	, _needRefresh(true)
{
	//0
	_vertices.push_back(-1.0f); _vertices.push_back(-1.0f); _vertices.push_back(0.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(0.0f); _vertices.push_back(1.0f);   //n
	_vertices.push_back(0.0f); _vertices.push_back(0.0f);                              //tc

	//1
	_vertices.push_back(1.0f); _vertices.push_back(-1.0f); _vertices.push_back(0.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(0.0f); _vertices.push_back(1.0f);   //n
	_vertices.push_back(1.0f); _vertices.push_back(0.0f);                              //tc

	//2
	_vertices.push_back(-1.0f); _vertices.push_back(1.0f); _vertices.push_back(0.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(0.0f); _vertices.push_back(1.0f);   //n
	_vertices.push_back(0.0f); _vertices.push_back(1.0f);                              //tc

	//3
	_vertices.push_back(1.0f); _vertices.push_back(1.0f); _vertices.push_back(0.0f); //v
	_vertices.push_back(0.0f); _vertices.push_back(0.0f); _vertices.push_back(1.0f);   //n
	_vertices.push_back(1.0f); _vertices.push_back(1.0f);                              //tc
}

veSurfaceRenderer::~veSurfaceRenderer()
{

}

void veSurfaceRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera)
{
	if (_technique) {
		updateBuffer();
		for (unsigned int i = 0; i < _technique->getPassNum(); ++i) {
			auto pass = _technique->getPass(i);
			if (camera->getMask() & pass->drawMask()) {
				bool isTransparent = pass->blendFunc() != veBlendFunc::DISABLE ? true : false;
				veRenderCommand rc;
				rc.pass = pass;
				rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix());
				//rc.attachedNode = node;
				rc.renderableObj = renderableObj;
				rc.camera = camera;
				rc.depthInCamera = (camera->viewMatrix() * rc.worldMatrix->value())[2][3];
				rc.drawFunc = VE_CALLBACK_1(veSurfaceRenderer::draw, this);
				pass->visit(rc);
				if (isTransparent)
					veRenderQueue::CURRENT_RENDER_QUEUE->pushCommand(veRenderQueue::RENDER_QUEUE_TRANSPARENT, rc);
				else
					veRenderQueue::CURRENT_RENDER_QUEUE->pushCommand(veRenderQueue::RENDER_QUEUE_ENTITY, rc);
			}
		}
	}
}

void veSurfaceRenderer::draw(const veRenderCommand &command)
{
	command.pass->apply(command);

	glBindVertexArray(_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void veSurfaceRenderer::updateBuffer()
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

		_needRefresh = false;
	}
}
