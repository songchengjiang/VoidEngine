#include "Mesh.h"
#include "MeshRenderer.h"

const unsigned int veMesh::MAX_ATTRIBUTE_NUM = 16;

veMesh::veMesh()
	: _needRefresh(true)
	, _vertexStride(0)
{
	_renderer = new veMeshRenderer;
}

veMesh::~veMesh()
{

}

unsigned int veMesh::getVertexStride()
{
	if (_needRefresh) {
		_vertexStride = 0;
		for (auto &attri : _attributes) {
			if (attri.valueType == veMesh::VertexAtrribute::FLOAT) _vertexStride += sizeof(GLfloat) * attri.valueNum;
			else if (attri.valueType == veMesh::VertexAtrribute::UINT) _vertexStride += sizeof(GLuint) * attri.valueNum;
			else if (attri.valueType == veMesh::VertexAtrribute::USHORT) _vertexStride += sizeof(GLushort) * attri.valueNum;
		}
	}
	return _vertexStride;
}

void veMesh::addVertexAtrribute(const VertexAtrribute &attri)
{
	veAssert(_attributes.size() < MAX_ATTRIBUTE_NUM);
	_attributes.push_back(attri);
	_needRefresh = true;
}

void veMesh::addPrimitive(const Primitive &primitive)
{
	_primitives.push_back(primitive);
	_needRefresh = true;
}

const veMesh::Primitive& veMesh::getPrimitive(unsigned int idx) const
{
	veAssert(idx < _primitives.size());
	return _primitives[idx];
}

void veMesh::addBone(veBone *bone)
{
	auto iter = std::find(_bones.begin(), _bones.end(), bone);
	if (iter != _bones.end()) return;
	_bones.push_back(bone);
	_needRefresh = true;
}

const veBone* veMesh::getBone(unsigned int idx) const
{
	veAssert(idx < _bones.size());
	return _bones[idx].get();
}

veBone* veMesh::getBone(unsigned int idx)
{
	veAssert(idx < _bones.size());
	return _bones[idx].get();
}

bool& veMesh::needRefresh()
{
	return _needRefresh;
}
