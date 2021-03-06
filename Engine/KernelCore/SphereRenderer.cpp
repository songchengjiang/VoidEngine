#include "SphereRenderer.h"
#include "RenderQueue.h"
#include "Node.h"
#include "Camera.h"

#define VERTEX_BUFFER_SIZE 440
#define INDEX_BUFFER_SIZE 240

static double sphere_vertices[VERTEX_BUFFER_SIZE] = {
	0.000000,0.000000,1.000000,0.000000,0.000000,1.000000,0.600000,1.000000
	,0.525731,0.000000,0.850651,0.525731,0.000000,0.850651,0.500000,0.823792
	,0.162460,0.500000,0.850651,0.162460,0.500000,0.850651,0.700000,0.823792
	,0.894427,0.000000,0.447214,0.894427,0.000000,0.447214,0.500000,0.647584
	,0.688191,0.500000,0.525731,0.688191,0.500000,0.525731,0.600000,0.676208
	,0.276393,0.850651,0.447214,0.276393,0.850651,0.447214,0.700000,0.647584
	,0.000000,0.000000,1.000000,0.000000,0.000000,1.000000,0.800000,1.000000
	,-0.425325,0.309017,0.850651,-0.425325,0.309017,0.850651,0.900000,0.823792
	,-0.262866,0.809017,0.525731,-0.262866,0.809017,0.525731,0.800000,0.676208
	,-0.723607,0.525731,0.447214,-0.723607,0.525731,0.447214,0.900000,0.647584
	,0.000000,0.000000,1.000000,0.000000,0.000000,1.000000,0.000000,1.000000
	,-0.425325,0.309017,0.850651,-0.425325,0.309017,0.850651,-0.100000,0.823792
	,-0.425325,-0.309017,0.850651,-0.425325,-0.309017,0.850651,0.100000,0.823792
	,-0.723607,0.525731,0.447214,-0.723607,0.525731,0.447214,-0.100000,0.647584
	,-0.850651,-0.000000,0.525731,-0.850651,-0.000000,0.525731,0.000000,0.676208
	,-0.723607,-0.525731,0.447214,-0.723607,-0.525731,0.447214,0.100000,0.647584
	,0.000000,0.000000,1.000000,0.000000,0.000000,1.000000,0.200000,1.000000
	,0.162460,-0.500000,0.850651,0.162460,-0.500000,0.850651,0.300000,0.823792
	,-0.262865,-0.809017,0.525731,-0.262865,-0.809017,0.525731,0.200000,0.676208
	,0.276393,-0.850651,0.447214,0.276393,-0.850651,0.447214,0.300000,0.647584
	,0.000000,0.000000,1.000000,0.000000,0.000000,1.000000,0.400000,1.000000
	,0.688191,-0.500000,0.525731,0.688191,-0.500000,0.525731,0.400000,0.676208
	,0.951057,-0.309017,0.000000,0.951057,-0.309017,0.000000,0.450000,0.500000
	,0.951056,0.309017,0.000000,0.951057,0.309017,0.000000,0.550000,0.500000
	,0.723607,-0.525731,-0.447214,0.723607,-0.525731,-0.447214,0.400000,0.352416
	,0.850651,0.000000,-0.525731,0.850651,0.000000,-0.525731,0.500000,0.323792
	,0.723607,0.525731,-0.447214,0.723607,0.525731,-0.447214,0.600000,0.352416
	,0.587785,0.809017,0.000000,0.587785,0.809017,-0.000000,0.650000,0.500000
	,-0.000000,1.000000,0.000000,-0.000000,1.000000,-0.000000,0.750000,0.500000
	,0.262866,0.809017,-0.525731,0.262866,0.809017,-0.525731,0.700000,0.323792
	,-0.276393,0.850651,-0.447214,-0.276393,0.850651,-0.447214,0.800000,0.352416
	,-0.587785,0.809017,0.000000,-0.587785,0.809017,0.000000,0.850000,0.500000
	,-0.951056,0.309017,0.000000,-0.951057,0.309017,-0.000000,0.950000,0.500000
	,-0.688191,0.500000,-0.525731,-0.688191,0.500000,-0.525731,0.900000,0.323792
	,-0.951056,0.309017,0.000000,-0.951057,0.309017,-0.000000,-0.050000,0.500000
	,-0.688191,0.500000,-0.525731,-0.688191,0.500000,-0.525731,-0.100000,0.323792
	,-0.894427,-0.000000,-0.447214,-0.894427,-0.000000,-0.447214,0.000000,0.352416
	,-0.951056,-0.309017,0.000000,-0.951056,-0.309017,-0.000000,0.050000,0.500000
	,-0.587785,-0.809017,0.000000,-0.587785,-0.809017,-0.000000,0.150000,0.500000
	,-0.688191,-0.500000,-0.525731,-0.688191,-0.500000,-0.525731,0.100000,0.323792
	,-0.276393,-0.850651,-0.447214,-0.276393,-0.850651,-0.447214,0.200000,0.352416
	,0.000000,-1.000000,0.000000,0.000000,-1.000000,-0.000000,0.250000,0.500000
	,0.587785,-0.809017,0.000000,0.587785,-0.809017,-0.000000,0.350000,0.500000
	,0.262866,-0.809017,-0.525731,0.262866,-0.809017,-0.525731,0.300000,0.323792
	,0.000000,0.000000,-1.000000,-0.000000,-0.000000,-1.000000,0.700000,0.000000
	,-0.162460,0.500000,-0.850651,-0.162460,0.500000,-0.850651,0.800000,0.176208
	,0.425325,0.309017,-0.850651,0.425325,0.309017,-0.850651,0.600000,0.176208
	,0.000000,0.000000,-1.000000,-0.000000,-0.000000,-1.000000,-0.100000,0.000000
	,-0.525731,-0.000000,-0.850651,-0.525731,-0.000000,-0.850651,0.000000,0.176208
	,-0.162460,0.500000,-0.850651,-0.162460,0.500000,-0.850651,-0.200000,0.176208
	,0.000000,0.000000,-1.000000,-0.000000,-0.000000,-1.000000,0.100000,0.000000
	,-0.162460,-0.500000,-0.850651,-0.162460,-0.500000,-0.850651,0.200000,0.176208
	,0.000000,0.000000,-1.000000,-0.000000,-0.000000,-1.000000,0.300000,0.000000
	,0.425326,-0.309017,-0.850651,0.425325,-0.309017,-0.850651,0.400000,0.176208
	,0.000000,0.000000,-1.000000,-0.000000,-0.000000,-1.000000,0.500000,0.000000
};

static uint16_t sphere_indices[INDEX_BUFFER_SIZE] = {
	0,1,2,1,3,4,1,4,2,20,17,1,17,21,1,1,21,3,3,22,23,23,4,3
	,22,3,21,2,4,5,27,5,4,27,4,23,22,25,23,23,25,26,26,27,23,22,24,25
	,24,22,42,22,21,42,46,26,25,46,25,53,53,25,24,27,26,29,46,29,26,5,27,28
	,27,29,28,28,29,30,45,30,29,45,29,46,44,45,46,54,46,53,45,33,30,31,30,33
	,30,31,28,31,8,28,28,8,5,9,31,32,31,33,32,31,9,8,2,5,8,2,8,7
	,7,8,9,6,2,7,52,53,51,53,24,43,53,43,51,50,51,48,51,40,39,51,39,48
	,51,43,40,41,40,43,41,43,42,42,43,24,38,39,40,40,41,38,37,36,39,37,39,38
	,48,39,36,15,37,38,41,18,38,38,18,15,19,41,42,41,19,18,42,21,19,17,18,19
	,17,19,21,12,15,18,12,18,17,12,14,15,37,15,14,10,11,12,11,14,12,16,12,17
	,11,13,14,37,14,34,34,14,13,36,37,34,34,35,36,48,36,35,48,35,49,47,48,49
};

veSphereRenderer::veSphereRenderer()
	: _needRefresh(true)
{
	for (unsigned int i = 0; i < VERTEX_BUFFER_SIZE; i += 8) {
		_vertices.push_back(sphere_vertices[i + 0]); _vertices.push_back(sphere_vertices[i + 1]); _vertices.push_back(sphere_vertices[i + 2]); //v
		_vertices.push_back(sphere_vertices[i + 3]); _vertices.push_back(sphere_vertices[i + 4]); _vertices.push_back(sphere_vertices[i + 5]);   //n
		_vertices.push_back(sphere_vertices[i + 6]); _vertices.push_back(sphere_vertices[i + 7]);                              //tc
	}

	for (unsigned int face = 0; face < INDEX_BUFFER_SIZE; face += 3) {
		_indices.push_back(sphere_indices[face + 0]); _indices.push_back(sphere_indices[face + 1]); _indices.push_back(sphere_indices[face + 2]);
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
    }, [](GLuint ibo){
        glDeleteBuffers(1, &ibo);
    });
}

veSphereRenderer::~veSphereRenderer()
{

}

void veSphereRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID)
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

void veSphereRenderer::draw(veRenderCommand &command)
{
	if (!isNeedRendering())
		return;
	if (!command.pass->apply(command))
		return;
	glBindVertexArray(_vaoBuffer->getData(command.contextID));
	glDrawElements(GL_TRIANGLES, GLsizei(_indices.size()), GL_UNSIGNED_SHORT, nullptr);
}

void veSphereRenderer::updateBuffer(unsigned int contextID)
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