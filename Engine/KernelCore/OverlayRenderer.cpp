#include "OverlayRenderer.h"
#include "RenderQueue.h"
#include "Visualiser.h"
#include "Node.h"
#include "Overlay.h"

veOverlayRenderer::veOverlayRenderer()
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

veOverlayRenderer::~veOverlayRenderer()
{

}

void veOverlayRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera)
{
	if (_technique) {
		veOverlay *overlay = static_cast<veOverlay *>(renderableObj);
		for (unsigned int i = 0; i < _technique->getPassNum(); ++i) {
			veRenderCommand rc;
			rc.pass = _technique->getPass(i);
			rc.attachedNode = node;
			rc.renderableObj = renderableObj;
			rc.camera = camera;
			rc.renderer = this;
			veRenderQueue::CURRENT_RENDER_QUEUE->pushCommand(veRenderQueue::RENDER_QUEUE_OVERLAY, rc);
		}
	}
}

void veOverlayRenderer::draw(const veRenderCommand &command)
{
	if (!command.pass) return;

	command.pass->apply(command);
	auto overlay = static_cast<veOverlay *>(command.renderableObj);

	if (!_vao) {
		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vbo);
	}

	glBindVertexArray(_vao);

	if (_needRefresh) {
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

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
