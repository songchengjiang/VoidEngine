#include "TerrainRenderer.h"
#include "RenderQueue.h"
#include "Node.h"
#include "MatrixPtr.h"
#include "Camera.h"
#include "Terrain.h"

veTerrainRenderer::veTerrainRenderer()
	: _needRefresh(true)
    , _indicesCount(0)
{
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

veTerrainRenderer::~veTerrainRenderer()
{

}

void veTerrainRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID)
{
    
    auto vao = _vaoBuffer->getData(contextID);
    if (!vao) {
        vao = _vaoBuffer->createData(contextID);
        _needRefresh = true;
    }
    
    auto terrainGrid = static_cast<veTerrainGrid *>(renderableObj);
    if (_needRefresh) {
        glBindVertexArray(vao);
        
        auto vbo = _vboBuffer->getData(contextID);
        if (!vbo){
            vbo = _vboBuffer->createData(contextID);
        }
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        
        if (!terrainGrid->getVertexArray()->empty())
            glBufferData(GL_ARRAY_BUFFER, terrainGrid->getVertexArray()->size() * sizeof((*terrainGrid->getVertexArray())[0]), terrainGrid->getVertexArray()->buffer(), GL_DYNAMIC_DRAW);
        
        unsigned int stride = terrainGrid->getVertexStride();
        
        unsigned int offsets = 0;
        for (unsigned int i = 0; i < terrainGrid->getVertexAtrributeNum(); ++i) {
            glVertexAttribPointer(i, terrainGrid->getVertexAtrributeSize(i), GL_FLOAT, GL_FALSE, stride, (GLvoid *)(sizeof(GLfloat) * offsets));
            glEnableVertexAttribArray(i);
            offsets += terrainGrid->getVertexAtrributeSize(i);
        }
        
        auto ibo = _iboBuffer->getData(contextID);
        if (!ibo){
            ibo = _iboBuffer->createData(contextID);
        }
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        if (!terrainGrid->getIndexArray()->empty())
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrainGrid->getIndexArray()->size() * sizeof((*terrainGrid->getIndexArray())[0]), terrainGrid->getIndexArray()->buffer(), GL_STATIC_DRAW);
        _indicesCount = (GLsizei)terrainGrid->getIndexArray()->size();
        
        _needRefresh = false;
    }

	veRenderCommand rc;
	rc.mask = node->getMask();
	rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix());
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

void veTerrainRenderer::draw(veRenderCommand &command)
{
	if (!isNeedRendering())
		return;
	if (!command.pass->apply(command))
		return;

	glBindVertexArray(_vaoBuffer->getData(command.contextID));
    if (0 < _indicesCount)
        glDrawElements(GL_TRIANGLE_STRIP, _indicesCount, GL_UNSIGNED_SHORT, nullptr);
}
