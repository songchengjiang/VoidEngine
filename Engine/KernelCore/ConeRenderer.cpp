#include "ConeRenderer.h"
#include "RenderQueue.h"
#include "Node.h"
#include "Camera.h"

#define VERTEX_BUFFER_SIZE 408
#define INDEX_BUFFER_SIZE 96

static double cone_vertices[VERTEX_BUFFER_SIZE] = {
	0.000000,0.000000,-1.000000,0.000000,0.000000,-1.000000,0.500000,0.000000
	,0.923880,0.382683,-1.000000,0.000000,0.000000,-1.000000,0.812500,0.000000
	,1.000000,0.000000,-1.000000,0.000000,0.000000,-1.000000,0.750000,0.000000
	,0.707107,0.707107,-1.000000,0.000000,0.000000,-1.000000,0.875000,0.000000
	,0.382683,0.923880,-1.000000,0.000000,0.000000,-1.000000,0.937500,0.000000
	,-0.000000,1.000000,-1.000000,0.000000,0.000000,-1.000000,0.000000,0.000000
	,0.382683,0.923880,-1.000000,0.000000,0.000000,-1.000000,-0.062500,0.000000
	,-0.382684,0.923880,-1.000000,0.000000,0.000000,-1.000000,0.062500,0.000000
	,-0.707107,0.707107,-1.000000,0.000000,0.000000,-1.000000,0.125000,0.000000
	,-0.923880,0.382683,-1.000000,0.000000,0.000000,-1.000000,0.187500,0.000000
	,-1.000000,0.000000,-1.000000,0.000000,0.000000,-1.000000,0.250000,0.000000
	,-0.923880,-0.382683,-1.000000,0.000000,0.000000,-1.000000,0.312500,0.000000
	,-0.707107,-0.707107,-1.000000,0.000000,0.000000,-1.000000,0.375000,0.000000
	,-0.382684,-0.923880,-1.000000,0.000000,0.000000,-1.000000,0.437500,0.000000
	,0.000000,-1.000000,-1.000000,0.000000,0.000000,-1.000000,0.500000,0.000000
	,0.382684,-0.923879,-1.000000,0.000000,0.000000,-1.000000,0.562500,0.000000
	,0.707107,-0.707107,-1.000000,0.000000,0.000000,-1.000000,0.625000,0.000000
	,0.923880,-0.382683,-1.000000,0.000000,0.000000,-1.000000,0.687500,0.000000
	,1.000000,0.000000,-1.000000,0.707107,0.000000,0.707107,0.750000,0.000000
	,0.923880,0.382683,-1.000000,0.653282,0.270598,0.707107,0.812500,0.000000
	,0.000000,0.000000,0.000000,0.700215,0.139281,0.700215,0.500000,1.000000
	,0.707107,0.707107,-1.000000,0.500000,0.500000,0.707107,0.875000,0.000000
	,0.000000,0.000000,0.000000,0.593613,0.396640,0.700215,0.500000,1.000000
	,0.382683,0.923880,-1.000000,0.270598,0.653282,0.707107,0.937500,0.000000
	,0.000000,0.000000,0.000000,0.396640,0.593613,0.700215,0.500000,1.000000
	,0.382683,0.923880,-1.000000,0.270598,0.653282,0.707107,-0.062500,0.000000
	,-0.000000,1.000000,-1.000000,0.000000,0.707107,0.707107,0.000000,0.000000
	,0.000000,0.000000,0.000000,0.139281,0.700215,0.700215,0.500000,1.000000
	,-0.382684,0.923880,-1.000000,-0.270598,0.653281,0.707107,0.062500,0.000000
	,0.000000,0.000000,0.000000,-0.139281,0.700215,0.700215,0.500000,1.000000
	,-0.707107,0.707107,-1.000000,-0.500000,0.500000,0.707107,0.125000,0.000000
	,0.000000,0.000000,0.000000,-0.396640,0.593613,0.700215,0.500000,1.000000
	,-0.923880,0.382683,-1.000000,-0.653282,0.270598,0.707107,0.187500,0.000000
	,0.000000,0.000000,0.000000,-0.593613,0.396640,0.700215,0.500000,1.000000
	,-1.000000,0.000000,-1.000000,-0.707107,0.000000,0.707107,0.250000,0.000000
	,0.000000,0.000000,0.000000,-0.700215,0.139281,0.700215,0.500000,1.000000
	,-0.923880,-0.382683,-1.000000,-0.653282,-0.270598,0.707107,0.312500,0.000000
	,0.000000,0.000000,0.000000,-0.700215,-0.139281,0.700215,0.500000,1.000000
	,-0.707107,-0.707107,-1.000000,-0.500000,-0.500000,0.707107,0.375000,0.000000
	,0.000000,0.000000,0.000000,-0.593613,-0.396639,0.700215,0.500000,1.000000
	,-0.382684,-0.923880,-1.000000,-0.270598,-0.653281,0.707107,0.437500,0.000000
	,0.000000,0.000000,0.000000,-0.396640,-0.593613,0.700215,0.500000,1.000000
	,0.000000,-1.000000,-1.000000,0.000000,-0.707107,0.707107,0.500000,0.000000
	,0.000000,0.000000,0.000000,-0.139281,-0.700215,0.700215,0.500000,1.000000
	,0.382684,-0.923879,-1.000000,0.270598,-0.653281,0.707107,0.562500,0.000000
	,0.000000,0.000000,0.000000,0.139281,-0.700215,0.700215,0.500000,1.000000
	,0.707107,-0.707107,-1.000000,0.500000,-0.500000,0.707107,0.625000,0.000000
	,0.000000,0.000000,0.000000,0.396640,-0.593613,0.700215,0.500000,1.000000
	,0.923880,-0.382683,-1.000000,0.653282,-0.270598,0.707107,0.687500,0.000000
	,0.000000,0.000000,0.000000,0.593613,-0.396639,0.700215,0.500000,1.000000
	,0.000000,0.000000,0.000000,0.700215,-0.139281,0.700215,0.500000,1.000000
};

static uint16_t cone_indices[INDEX_BUFFER_SIZE] = {
	0,1,2,0,3,1,0,4,3,0,5,6,0,7,5,0,8,7,0,9,8,0,10,9
	,0,11,10,0,12,11,0,13,12,0,14,13,0,15,14,0,16,15,0,17,16,0,2,17
	,18,19,20,48,18,50,19,21,22,21,23,24,46,48,49,44,46,47,42,44,45,40,42,43
	,38,40,41,36,38,39,34,36,37,32,34,35,30,32,33,28,30,31,26,28,29,25,26,27
};

veConeRenderer::veConeRenderer()
	: _vao(0)
	, _vbo(0)
	, _ibo(0)
	, _needRefresh(true)
{
	for (unsigned int i = 0; i < VERTEX_BUFFER_SIZE; i += 8) {
		_vertices.push_back(cone_vertices[i + 0]); _vertices.push_back(cone_vertices[i + 1]); _vertices.push_back(cone_vertices[i + 2]); //v
		_vertices.push_back(cone_vertices[i + 3]); _vertices.push_back(cone_vertices[i + 4]); _vertices.push_back(cone_vertices[i + 5]);   //n
		_vertices.push_back(cone_vertices[i + 6]); _vertices.push_back(cone_vertices[i + 7]);                              //tc
	}

	for (unsigned int face = 0; face < INDEX_BUFFER_SIZE; face += 3) {
		_indices.push_back(cone_indices[face + 0]); _indices.push_back(cone_indices[face + 1]); _indices.push_back(cone_indices[face + 2]);
	}
}

veConeRenderer::~veConeRenderer()
{

}

void veConeRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera)
{
	updateBuffer();

	veRenderCommand rc;
	rc.mask = node->getMask();
	rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix());
	//rc.attachedNode = node;
	rc.camera = camera;
	rc.sceneManager = camera->getSceneManager();
	rc.depthInCamera = (camera->viewMatrix() * rc.worldMatrix->value())[2][3];
	rc.renderer = this;

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

void veConeRenderer::draw(veRenderCommand &command)
{
	if (!isNeedRendering())
		return;
	if (!command.pass->apply(command))
		return;
	glBindVertexArray(_vao);
	glDrawElements(GL_TRIANGLES, GLsizei(_indices.size()), GL_UNSIGNED_SHORT, nullptr);
}

void veConeRenderer::updateBuffer()
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