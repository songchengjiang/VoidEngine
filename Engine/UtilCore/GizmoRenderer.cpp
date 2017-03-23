#include "GizmoRenderer.h"
#include "KernelCore/Camera.h"
#include "Gizmo.h"

veGizmoRenderer::veGizmoRenderer()
    : _refresh(true)
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
    
    _iboBuffers[LINES_INDICES] = veGLDataBufferManager::instance()->createGLDataBuffer([]() -> GLuint{
        GLuint ibo;
        glGenBuffers(1, &ibo);
        return ibo;
    }, [](GLuint ibo){
        glDeleteBuffers(1, &ibo);
    });
    
    _iboBuffers[TRIANGLES_INDICES] = veGLDataBufferManager::instance()->createGLDataBuffer([]() -> GLuint{
        GLuint ibo;
        glGenBuffers(1, &ibo);
        return ibo;
    }, [](GLuint ibo){
        glDeleteBuffers(1, &ibo);
    });
}

veGizmoRenderer::~veGizmoRenderer()
{

}

void veGizmoRenderer::render(veNode *node, veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID)
{
    auto vao = _vaoBuffer->getData(contextID);
    if ((_refresh || !vao) && !_vertices.empty()){
        if (!vao) {
            vao = _vaoBuffer->createData(contextID);
        }
        glBindVertexArray(vao);
        
        auto vbo = _vboBuffer->getData(contextID);
        if (!vbo) {
            vbo = _vboBuffer->createData(contextID);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            unsigned int stride = sizeof(GLfloat) * getVertexStride();
            //v
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
            glEnableVertexAttribArray(0);
            
            //col
            glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(sizeof(GLfloat) * 3));
            glEnableVertexAttribArray(1);
        }
        
        {
            std::unique_lock<std::mutex> lock(_dataMutex);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(_vertices[0]), _vertices.buffer(), GL_STATIC_DRAW);
            
            if (!_indices[LINES_INDICES].empty()){
                auto ibo = _iboBuffers[LINES_INDICES]->getData(contextID);
                if (!ibo) {
                    ibo = _iboBuffers[LINES_INDICES]->createData(contextID);
                }
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices[LINES_INDICES].size() * sizeof(_indices[LINES_INDICES][0]), _indices[LINES_INDICES].buffer(), GL_STATIC_DRAW);
            }
            
            if (!_indices[TRIANGLES_INDICES].empty()){
                auto ibo = _iboBuffers[TRIANGLES_INDICES]->getData(contextID);
                if (!ibo) {
                    ibo = _iboBuffers[TRIANGLES_INDICES]->createData(contextID);
                }
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices[TRIANGLES_INDICES].size() * sizeof(_indices[TRIANGLES_INDICES][0]), _indices[TRIANGLES_INDICES].buffer(), GL_STATIC_DRAW);
            }
        }
        
        _refresh = false;
    }
    
    veRenderCommand rc;
    rc.mask = node->getMask();
    rc.worldMatrix = new veMat4Ptr(node->getNodeToWorldMatrix() * veMat4::scale(static_cast<veGizmo *>(renderableObj)->getScale()));
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

void veGizmoRenderer::draw(veRenderCommand &command)
{
    if (!command.pass->apply(command))
        return;
    glBindVertexArray(_vaoBuffer->getData(command.contextID));
    
    if (!_indices[LINES_INDICES].empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iboBuffers[LINES_INDICES]->getData(command.contextID));
        glDrawElements(GL_LINES, GLsizei(_indices[LINES_INDICES].size()), GL_UNSIGNED_SHORT, nullptr);
    }
    
    if (!_indices[TRIANGLES_INDICES].empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _iboBuffers[TRIANGLES_INDICES]->getData(command.contextID));
        glDrawElements(GL_TRIANGLES, GLsizei(_indices[TRIANGLES_INDICES].size()), GL_UNSIGNED_SHORT, nullptr);
    }
}

void veGizmoRenderer::drawLines(const veVec3 &start, const veVec3 &end, const veVec4 &color)
{
    std::unique_lock<std::mutex> lock(_dataMutex);
    size_t vIdx = _vertices.size() / getVertexStride();
    _vertices.push_back(start.x()); _vertices.push_back(start.y()); _vertices.push_back(start.z());
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _vertices.push_back(end.x()); _vertices.push_back(end.y()); _vertices.push_back(end.z());
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _indices[LINES_INDICES].push_back(vIdx);
    _indices[LINES_INDICES].push_back(vIdx + 1);
}

void veGizmoRenderer::drawTri(const veVec3 &v0, const veVec3 &v1, const veVec3 &v2, const veVec4 &color)
{
    std::unique_lock<std::mutex> lock(_dataMutex);
    size_t vIdx = _vertices.size() / getVertexStride();
    _vertices.push_back(v0.x()); _vertices.push_back(v0.y()); _vertices.push_back(v0.z());
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _vertices.push_back(v1.x()); _vertices.push_back(v1.y()); _vertices.push_back(v1.z());
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _vertices.push_back(v2.x()); _vertices.push_back(v2.y()); _vertices.push_back(v2.z());
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _indices[TRIANGLES_INDICES].push_back(vIdx);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 1);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 2);
}

void veGizmoRenderer::drawQuat(const veVec3 &bl, const veVec3 &br, const veVec3 &tr, const veVec3 &tl, const veVec4 &color)
{
    std::unique_lock<std::mutex> lock(_dataMutex);
    size_t vIdx = _vertices.size() / getVertexStride();
    _vertices.push_back(bl.x()); _vertices.push_back(bl.y()); _vertices.push_back(bl.z());
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _vertices.push_back(br.x()); _vertices.push_back(br.y()); _vertices.push_back(br.z());
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _vertices.push_back(tr.x()); _vertices.push_back(tr.y()); _vertices.push_back(tr.z());
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _vertices.push_back(tl.x()); _vertices.push_back(tl.y()); _vertices.push_back(tl.z());
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _indices[TRIANGLES_INDICES].push_back(vIdx);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 2);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 3);
    
    _indices[TRIANGLES_INDICES].push_back(vIdx);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 1);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 2);
}

void veGizmoRenderer::drawCircle(const veVec3 &axes, veReal radius, const veVec4 &color)
{
    std::unique_lock<std::mutex> lock(_dataMutex);
    size_t vIdx = _vertices.size() / getVertexStride();
#define CIRCLE_SEGMENTS 64
    
    veReal angleStep = veMath::TWO_PI / CIRCLE_SEGMENTS;
    veReal angle = 0.0f;
    for (unsigned short i = 0; i < CIRCLE_SEGMENTS; ++i){
        float x = radius * veMath::veCos(angle);
        float y = radius * veMath::veSin(angle);
        veVec3 vertex;
        if (axes == veVec3::UNIT_X){
            vertex = veVec3(0.0f, x, y);
        }else if (axes == veVec3::UNIT_Y){
            vertex = veVec3(x, 0.0f, y);
        }else{
            vertex = veVec3(x, y, 0.0f);
        }
        _vertices.push_back(vertex.x()); _vertices.push_back(vertex.y()); _vertices.push_back(vertex.z());
        _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
        angle += angleStep;
    }
    
    for (unsigned short i = 0; i < CIRCLE_SEGMENTS; ++i) {
        _indices[LINES_INDICES].push_back(vIdx + i);
        _indices[LINES_INDICES].push_back((vIdx + (i + 1) % CIRCLE_SEGMENTS));
    }
    
#undef CIRCLE_SEGMENTS
}

void veGizmoRenderer::drawCone(const veVec3 &pos, const veVec3 &axes, veReal radius, veReal height, const veVec4 &color)
{
    std::unique_lock<std::mutex> lock(_dataMutex);
    size_t vIdx = _vertices.size() / getVertexStride();
#define CONE_SEGMENTS 20
    
    veVec3 top = pos + axes * height;
    _vertices.push_back(top.x()); _vertices.push_back(top.y()); _vertices.push_back(top.z());
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    veReal angleStep = veMath::TWO_PI / CONE_SEGMENTS;
    veReal angle = 0.0f;
    for (unsigned short i = 0; i < CONE_SEGMENTS; ++i) {
        float x = radius * veMath::veCos(angle);
        float y = radius * veMath::veSin(angle);
        veVec3 vertex;
        if (axes == veVec3::UNIT_X){
            vertex = veVec3(pos.x(), x, y);
        }else if (axes == veVec3::UNIT_Y){
            vertex = veVec3(x, pos.y(), y);
        }else{
            vertex = veVec3(x, y, pos.z());
        }
        _vertices.push_back(vertex.x()); _vertices.push_back(vertex.y()); _vertices.push_back(vertex.z());
        _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
        angle += angleStep;
    }
    
    for (unsigned short i = 0; i < CONE_SEGMENTS; ++i) {
        _indices[TRIANGLES_INDICES].push_back(vIdx);
        _indices[TRIANGLES_INDICES].push_back(vIdx + i + 1);
        _indices[TRIANGLES_INDICES].push_back(CONE_SEGMENTS < (i + 2)? vIdx + 1: vIdx + i + 2);
    }
    
#undef CONE_SEGMENTS
}

void veGizmoRenderer::drawBox(const veVec3 &pos, veReal halfSize, const veVec4 &color)
{
    std::unique_lock<std::mutex> lock(_dataMutex);
    size_t vIdx = _vertices.size() / getVertexStride();
    
    _vertices.push_back(pos.x() - halfSize); _vertices.push_back(pos.y() - halfSize); _vertices.push_back(pos.z() + halfSize);
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _vertices.push_back(pos.x() + halfSize); _vertices.push_back(pos.y() - halfSize); _vertices.push_back(pos.z() + halfSize);
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _vertices.push_back(pos.x() + halfSize); _vertices.push_back(pos.y() + halfSize); _vertices.push_back(pos.z() + halfSize);
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _vertices.push_back(pos.x() - halfSize); _vertices.push_back(pos.y() + halfSize); _vertices.push_back(pos.z() + halfSize);
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _vertices.push_back(pos.x() + halfSize); _vertices.push_back(pos.y() - halfSize); _vertices.push_back(pos.z() - halfSize);
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _vertices.push_back(pos.x() - halfSize); _vertices.push_back(pos.y() - halfSize); _vertices.push_back(pos.z() - halfSize);
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _vertices.push_back(pos.x() - halfSize); _vertices.push_back(pos.y() + halfSize); _vertices.push_back(pos.z() - halfSize);
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _vertices.push_back(pos.x() + halfSize); _vertices.push_back(pos.y() + halfSize); _vertices.push_back(pos.z() - halfSize);
    _vertices.push_back(color.x()); _vertices.push_back(color.y()); _vertices.push_back(color.z()); _vertices.push_back(color.w());
    
    _indices[TRIANGLES_INDICES].push_back(vIdx);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 2);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 3);
    _indices[TRIANGLES_INDICES].push_back(vIdx);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 1);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 2);
    
    _indices[TRIANGLES_INDICES].push_back(vIdx + 1);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 7);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 2);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 1);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 4);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 7);
    
    _indices[TRIANGLES_INDICES].push_back(vIdx + 4);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 6);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 7);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 4);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 5);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 6);
    
    _indices[TRIANGLES_INDICES].push_back(vIdx + 5);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 3);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 6);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 5);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 0);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 3);
    
    _indices[TRIANGLES_INDICES].push_back(vIdx + 3);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 7);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 6);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 3);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 2);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 7);
    
    _indices[TRIANGLES_INDICES].push_back(vIdx + 5);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 1);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 0);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 5);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 4);
    _indices[TRIANGLES_INDICES].push_back(vIdx + 1);

}

unsigned int veGizmoRenderer::getVertexStride() const
{
    return 3 + 4;
}

void veGizmoRenderer::refresh()
{
    std::unique_lock<std::mutex> lock(_dataMutex);
    _vertices.clear();
    _indices[LINES_INDICES].clear();
    _indices[TRIANGLES_INDICES].clear();
    _refresh = true;
}
