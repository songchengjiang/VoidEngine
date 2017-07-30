#include "SkyBox.h"
#include "Camera.h"
#include "SceneManager.h"
#include "MeshRenderer.h"
#include "Viewer.h"

veSkyBox::veSkyBox(veReal size)
	: USE_VE_PTR_INIT
{
	_renderer = new veMeshRenderer();
	setSize(size);
    
    _vertices = new veRealArray();
    //NEGATIVE_X
    _vertices->push_back(-1.0f); _vertices->push_back(-1.0f); _vertices->push_back(-1.0f); //v
    _vertices->push_back(-1.0f); _vertices->push_back(0.0f); _vertices->push_back(0.0f);   //n
    _vertices->push_back(0.0f); _vertices->push_back(0.0f);                              //tc
    
    _vertices->push_back(-1.0f); _vertices->push_back(1.0f); _vertices->push_back(-1.0f); //v
    _vertices->push_back(-1.0f); _vertices->push_back(0.0f); _vertices->push_back(0.0f);   //n
    _vertices->push_back(0.0f); _vertices->push_back(1.0f);                              //tc
    
    _vertices->push_back(-1.0f); _vertices->push_back(-1.0f); _vertices->push_back(1.0f); //v
    _vertices->push_back(-1.0f); _vertices->push_back(0.0f); _vertices->push_back(0.0f);   //n
    _vertices->push_back(1.0f); _vertices->push_back(0.0f);                              //tc
    
    _vertices->push_back(-1.0f); _vertices->push_back(1.0f); _vertices->push_back(1.0f); //v
    _vertices->push_back(-1.0f); _vertices->push_back(0.0f); _vertices->push_back(0.0f);   //n
    _vertices->push_back(1.0f); _vertices->push_back(1.0f);                              //tc
    
    //POSITIVE_Z
    _vertices->push_back(-1.0f); _vertices->push_back(-1.0f); _vertices->push_back(1.0f); //v
    _vertices->push_back(0.0f); _vertices->push_back(0.0f); _vertices->push_back(1.0f);   //n
    _vertices->push_back(0.0f); _vertices->push_back(0.0f);                              //tc
    
    _vertices->push_back(-1.0f); _vertices->push_back(1.0f); _vertices->push_back(1.0f); //v
    _vertices->push_back(0.0f); _vertices->push_back(0.0f); _vertices->push_back(1.0f);   //n
    _vertices->push_back(0.0f); _vertices->push_back(1.0f);                                 //tc
    
    _vertices->push_back(1.0f); _vertices->push_back(-1.0f); _vertices->push_back(1.0f); //v
    _vertices->push_back(0.0f); _vertices->push_back(0.0f); _vertices->push_back(1.0f);   //n
    _vertices->push_back(1.0f); _vertices->push_back(0.0f);                              //tc
    
    _vertices->push_back(1.0f); _vertices->push_back(1.0f); _vertices->push_back(1.0f); //v
    _vertices->push_back(0.0f); _vertices->push_back(0.0f); _vertices->push_back(1.0f);   //n
    _vertices->push_back(1.0f); _vertices->push_back(1.0f);                              //tc
    
    //POSITIVE_X
    _vertices->push_back(1.0f); _vertices->push_back(-1.0f); _vertices->push_back(1.0f); //v
    _vertices->push_back(1.0f); _vertices->push_back(0.0f); _vertices->push_back(1.0f);   //n
    _vertices->push_back(0.0f); _vertices->push_back(0.0f);                              //tc
    
    _vertices->push_back(1.0f); _vertices->push_back(1.0f); _vertices->push_back(1.0f); //v
    _vertices->push_back(1.0f); _vertices->push_back(0.0f); _vertices->push_back(1.0f);   //n
    _vertices->push_back(0.0f); _vertices->push_back(1.0f);                                 //tc
    
    _vertices->push_back(1.0f); _vertices->push_back(-1.0f); _vertices->push_back(-1.0f); //v
    _vertices->push_back(1.0f); _vertices->push_back(0.0f); _vertices->push_back(1.0f);   //n
    _vertices->push_back(1.0f); _vertices->push_back(0.0f);                              //tc
    
    _vertices->push_back(1.0f); _vertices->push_back(1.0f); _vertices->push_back(-1.0f); //v
    _vertices->push_back(1.0f); _vertices->push_back(0.0f); _vertices->push_back(1.0f);   //n
    _vertices->push_back(1.0f); _vertices->push_back(1.0f);                              //tc
    
    //NEGATIVE_Z
    _vertices->push_back(1.0f); _vertices->push_back(-1.0f); _vertices->push_back(-1.0f); //v
    _vertices->push_back(0.0f); _vertices->push_back(0.0f); _vertices->push_back(-1.0f);   //n
    _vertices->push_back(0.0f); _vertices->push_back(0.0f);                              //tc
    
    _vertices->push_back(1.0f); _vertices->push_back(1.0f); _vertices->push_back(-1.0f); //v
    _vertices->push_back(0.0f); _vertices->push_back(0.0f); _vertices->push_back(-1.0f);   //n
    _vertices->push_back(0.0f); _vertices->push_back(1.0f);                                 //tc
    
    _vertices->push_back(-1.0f); _vertices->push_back(-1.0f); _vertices->push_back(-1.0f); //v
    _vertices->push_back(0.0f); _vertices->push_back(0.0f); _vertices->push_back(-1.0f);   //n
    _vertices->push_back(1.0f); _vertices->push_back(0.0f);                              //tc
    
    _vertices->push_back(-1.0f); _vertices->push_back(1.0f); _vertices->push_back(-1.0f); //v
    _vertices->push_back(0.0f); _vertices->push_back(0.0f); _vertices->push_back(-1.0f);   //n
    _vertices->push_back(1.0f); _vertices->push_back(1.0f);                              //tc
    
    //POSITIVE_Y
    _vertices->push_back(-1.0f); _vertices->push_back(1.0f); _vertices->push_back(1.0f); //v
    _vertices->push_back(0.0f); _vertices->push_back(1.0f); _vertices->push_back(0.0f);   //n
    _vertices->push_back(0.0f); _vertices->push_back(0.0f);                              //tc
    
    _vertices->push_back(-1.0f); _vertices->push_back(1.0f); _vertices->push_back(-1.0f); //v
    _vertices->push_back(0.0f); _vertices->push_back(1.0f); _vertices->push_back(0.0f);   //n
    _vertices->push_back(0.0f); _vertices->push_back(1.0f);                                 //tc
    
    _vertices->push_back(1.0f); _vertices->push_back(1.0f); _vertices->push_back(1.0f); //v
    _vertices->push_back(0.0f); _vertices->push_back(1.0f); _vertices->push_back(0.0f);   //n
    _vertices->push_back(1.0f); _vertices->push_back(0.0f);                              //tc
    
    _vertices->push_back(1.0f); _vertices->push_back(1.0f); _vertices->push_back(-1.0f); //v
    _vertices->push_back(0.0f); _vertices->push_back(1.0f); _vertices->push_back(0.0f);   //n
    _vertices->push_back(1.0f); _vertices->push_back(1.0f);                              //tc
    
    //NEGATIVE_Y
    _vertices->push_back(-1.0f); _vertices->push_back(-1.0f); _vertices->push_back(-1.0f); //v
    _vertices->push_back(0.0f); _vertices->push_back(-1.0f); _vertices->push_back(0.0f);   //n
    _vertices->push_back(0.0f); _vertices->push_back(0.0f);                              //tc
    
    _vertices->push_back(-1.0f); _vertices->push_back(-1.0f); _vertices->push_back(1.0f); //v
    _vertices->push_back(0.0f); _vertices->push_back(-1.0f); _vertices->push_back(0.0f);   //n
    _vertices->push_back(0.0f); _vertices->push_back(1.0f);                                 //tc
    
    _vertices->push_back(1.0f); _vertices->push_back(-1.0f); _vertices->push_back(-1.0f); //v
    _vertices->push_back(0.0f); _vertices->push_back(-1.0f); _vertices->push_back(0.0f);   //n
    _vertices->push_back(1.0f); _vertices->push_back(0.0f);                              //tc
    
    _vertices->push_back(1.0f); _vertices->push_back(-1.0f); _vertices->push_back(1.0f); //v
    _vertices->push_back(0.0f); _vertices->push_back(-1.0f); _vertices->push_back(0.0f);   //n
    _vertices->push_back(1.0f); _vertices->push_back(1.0f);                              //tc
    
    _indices = new veUint16Array;
    for (unsigned int face = 0; face < 6; ++face) {
        unsigned int idx = 4 * face;
        _indices->push_back(idx + 0); _indices->push_back(idx + 2); _indices->push_back(idx + 1);
        _indices->push_back(idx + 1); _indices->push_back(idx + 2); _indices->push_back(idx + 3);
    }
}

veSkyBox::~veSkyBox()
{

}

void veSkyBox::setSize(veReal size)
{
	_size = size;
}

void veSkyBox::beforeRender(veSceneManager *sm, veViewer *viewer)
{
    veCamera *camera = _attachedNode->getComponent<veCamera>();
    if (!camera)
        return;
    
    if (!_mesh.valid()) {
        _mesh = sm->createMesh("_SkyBoxMesh_");
        _mesh->setVertexArray(_vertices.get());
        _mesh->addVertexAtrribute({veMesh::VertexAtrribute::VERTEX_ATTRIB_POSITION, 3, veMesh::VertexAtrribute::FLOAT});
        _mesh->addVertexAtrribute({veMesh::VertexAtrribute::VERTEX_ATTRIB_NORMAL, 3, veMesh::VertexAtrribute::FLOAT});
        _mesh->addVertexAtrribute({veMesh::VertexAtrribute::VERTEX_ATTRIB_TEX_COORD0, 2, veMesh::VertexAtrribute::FLOAT});
        
        _mesh->addPrimitive({veMesh::Primitive::TRIANGLES, _indices});
        _mesh->setMaterial(_material.get());
    }
    
    veMat4 cameraWorldMat = _attachedNode->getNodeToWorldMatrix();
    veRenderCommand rc;
    veMat4 mat = veMat4::IDENTITY;
    mat[0][3] = cameraWorldMat[0][3];
    mat[1][3] = cameraWorldMat[1][3];
    mat[2][3] = cameraWorldMat[2][3];
    mat[0][0] = mat[1][1] = mat[2][2] = _size * 0.5f;
    
    _renderer->render(_mesh.get(), camera, mat, viewer->getContextID(), veRenderQueue::RENDER_QUEUE_BACKGROUND);
}
