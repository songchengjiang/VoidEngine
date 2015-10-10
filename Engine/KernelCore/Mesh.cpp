#include "Mesh.h"
#include "MeshRenderer.h"
#include "Node.h"

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

void veMesh::update(veNode *node, veVisualiser *vs)
{
	if (!_isVisible) return;
	updateBones(node);
	if (_renderer.valid())
		_renderer->visit(node, this, vs);
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

const veMesh::Primitive& veMesh::getPrimitive(size_t idx) const
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

void veMesh::caculateBoundingBox()
{
	if (!_attributes.empty()) {
		unsigned int vOffset = 0;
		unsigned int biOffset = 0;
		unsigned int bwOffset = 0;
		unsigned int bwNum = 0;
		unsigned int stride = 0;
		for (auto iter : _attributes) {
			if (iter.attributeType == VertexAtrribute::VERTEX_ATTRIB_POSITION) {
				vOffset = stride;
			}
			else if (iter.attributeType == VertexAtrribute::VERTEX_ATTRIB_BONE_INDICES) {
				biOffset = stride;
			}
			else if (iter.attributeType == VertexAtrribute::VERTEX_ATTRIB_BONE_WEIGHTS) {
				bwOffset = stride;
				bwNum = iter.valueNum;
			}
			if (iter.valueType == veMesh::VertexAtrribute::FLOAT) stride += sizeof(GLfloat) * iter.valueNum / sizeof(GLfloat);
			else if (iter.valueType == veMesh::VertexAtrribute::UINT) stride += sizeof(GLuint) * iter.valueNum / sizeof(GLfloat);
			else if (iter.valueType == veMesh::VertexAtrribute::USHORT) stride += sizeof(GLushort) * iter.valueNum / sizeof(GLfloat);
		}
		_boundingBox.dirty();
		for (size_t i = 0; i < _vertices->size(); i += stride) {
			_boundingBox.expandBy(veVec3(_vertices->buffer()[vOffset + i], _vertices->buffer()[vOffset + i + 1], _vertices->buffer()[vOffset + i + 2]));
		}

		if (!_bones.empty()) {
			veBoundingBox *bboxs = new veBoundingBox[_bones.size()];
			for (size_t i = 0; i < _vertices->size(); i += stride) {
				for (unsigned int bw = 0; bw < bwNum; ++bw) {
					auto boneWeight = _vertices->buffer()[bwOffset + i + bw];
					if (boneWeight != 0) {
						auto bone = (unsigned int)_vertices->buffer()[biOffset + i + bw];
						bboxs[bone].expandBy(veVec3(_vertices->buffer()[vOffset + i], _vertices->buffer()[vOffset + i + 1], _vertices->buffer()[vOffset + i + 2]));
					}
				}
			}
			for (size_t i = 0; i < _bones.size(); ++i) {
				_bones[i]->setBindPosBoundingBox(bboxs[i]);
			}

			delete[] bboxs;
		}
	}
}

bool& veMesh::needRefresh()
{
	return _needRefresh;
}

void veMesh::updateBones(veNode *node)
{
	if (!_bones.empty()) {
		_boundingBox.dirty();
		for (auto &iter : _bones) {
			veMat4 boneMat = node->getWorldToNodeMatrix() * iter->getBoneNode()->getNodeToWorldMatrix() * iter->getOffsetMat();
			iter->setBoundingBox(iter->getBindPosBoundingBox() * boneMat);
			_boundingBox.expandBy(iter->getBoundingBox());
		}
	}
}
