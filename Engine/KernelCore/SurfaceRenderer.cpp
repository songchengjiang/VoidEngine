#include "SurfaceRenderer.h"
#include "RenderQueue.h"
#include "Node.h"
#include "MatrixPtr.h"
#include "Camera.h"

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
	if (!isNeedRendering())
		return;
	updateBuffer();

	veRenderCommand rc;
	rc.mask = node->getMask();
	rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix());
	rc.renderableObj = renderableObj;
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.depthInCamera = (camera->viewMatrix() * rc.worldMatrix->value())[2][3];
	rc.renderer = this;

	auto materials = renderableObj->getMaterialArray();
	for (unsigned int mat = 0; mat < materials->getMaterialNum(); ++mat) {
		auto material = materials->getMaterial(mat);
		for (unsigned int i = 0; i < material->activeTechnique()->getPassNum(); ++i) {
			auto pass = material->activeTechnique()->getPass(i);
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

void veSurfaceRenderer::draw(const veRenderCommand &command)
{
	if (!command.pass->apply(command))
		return;

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
