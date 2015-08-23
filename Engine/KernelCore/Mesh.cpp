#include "Mesh.h"
#include "MeshRenderer.h"

const unsigned int veMesh::MAX_ATTRIBUTE_NUM = 16;

veMesh::veMesh()
	: _needRefresh(true)
{
	_renderer = new veMeshRenderer;
}

veMesh::~veMesh()
{

}

void veMesh::addVertexAtrribute(const VertexAtrribute &attri)
{
	veAssert(_attributes.size() < MAX_ATTRIBUTE_NUM);
	_attributes.push_back(attri);
}

void veMesh::addPrimitive(const Primitive &primitive)
{
	_primitives.push_back(primitive);
}

const veMesh::Primitive& veMesh::getPrimitive(unsigned int idx) const
{
	veAssert(idx < _primitives.size());
	return _primitives[idx];
}

bool& veMesh::needRefresh()
{
	return _needRefresh;
}

void veMesh::updateImp(veNode *node, veVisualiser *vs)
{

}
