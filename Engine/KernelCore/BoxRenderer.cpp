#include "BoxRenderer.h"
#include "RenderQueue.h"
#include "Node.h"
#include "Camera.h"

veBoxRenderer::veBoxRenderer()
	: _needRefresh(true)
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
    
    _iboBuffer = veGLDataBufferManager::instance()->createGLDataBuffer([]() -> GLuint{
        GLuint ibo;
        glGenBuffers(1, &ibo);
        return ibo;
    }, [](GLuint vbo){
        glDeleteBuffers(1, &vbo);
    });
}

veBoxRenderer::~veBoxRenderer()
{

}

void veBoxRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID)
{
	updateBuffer(contextID);

	veRenderCommand rc;
	rc.mask = node->getMask();
	rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix());
	//rc.attachedNode = node;
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.depthInCamera = (camera->viewMatrix() * rc.worldMatrix->value())[2][3];
	rc.renderer = this;
    rc.contextID = contextID;

    auto material = renderableObj->getMaterial();
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

void veBoxRenderer::draw(veRenderCommand &command)
{
	if (!isNeedRendering())
		return;
	if (!command.pass->apply(command))
		return;
	glBindVertexArray(_vaoBuffer->getData(command.contextID));
	glDrawElements(GL_TRIANGLES, GLsizei(_indices.size()), GL_UNSIGNED_SHORT, nullptr);
}

void veBoxRenderer::updateBuffer(unsigned int contextID)
{
    auto vao = _vaoBuffer->getData(contextID);
    if (!vao) {
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

        auto ibo = _iboBuffer->getData(contextID);
        if (!ibo) {
            ibo = _iboBuffer->createData(contextID);
        }

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(_indices[0]), _indices.buffer(), GL_STATIC_DRAW);

		_needRefresh = false;
	}
}
