#include "Mesh.h"
#include "MeshRenderer.h"
#include "Ray.h"
#include "Node.h"
#include <algorithm>

const unsigned int veMesh::MAX_ATTRIBUTE_NUM = 16;

veMesh::veMesh(veSceneManager *sm)
	: veRenderableObject(sm)
	, _needRefresh(false)
	, _vertexStride(0)
	, _transformFeedbackVertices(nullptr)
	, _transformFeedbackBuffer(0)
	, _transformFeedbackBufferSize(0)
{
    _renderer = new veMeshRenderer;
}

veMesh::~veMesh()
{

}

void veMesh::render(veNode *node, veCamera *camera, unsigned int contextID)
{
    generateTransformFeedbackBuffer(contextID);
    veRenderableObject::render(node, camera, contextID);
    _needRefresh = false;
}

void veMesh::update(veNode *node, veSceneManager *sm)
{
    updateBoundingBox();
    veRenderableObject::update(node, sm);
}

bool veMesh::intersectWith(veRay *ray, veNode *node)
{
    bool state = false;

    veMat4 wton = node->getWorldToNodeMatrix();
    veVec3 start = ray->getStart();
    veVec3 end = ray->getEnd();
    veVec3 localStart = wton * start;
    veVec3 localEnd = wton * end;
    ray->setStart(localStart);
    ray->setEnd(localEnd);
    if (ray->isIntersectWith(_boundingBox)) {
        veRay::Intersection inters;
        if (intersectWith(ray, inters.localPosition, inters.localNormal)) {
            veMat4 ntow = node->getNodeToWorldMatrix();
            veMat3 normMat(ntow[0][0], ntow[0][1], ntow[0][2]
                           , ntow[1][0], ntow[1][1], ntow[1][2]
                           , ntow[2][0], ntow[2][1], ntow[2][2]);
            normMat.inverse();
            normMat.transpose();
            inters.worldPosition = ntow * inters.localPosition;
            inters.worldNormal = normMat * inters.localNormal;
            inters.worldNormal.normalize();
            inters.node = node;
            inters.renderable = this;
            ray->addIntersection(inters);
            state = true;
        }
    }
    
    ray->setStart(start);
    ray->setEnd(end);
    
    return state;
}

//void veMesh::update(veNode *node, veSceneManager *sm)
//{
//	if (!_isVisible) return;
//	updateBones(node);
//	if (_renderer.valid())
//		_renderer->visit(node, this, sm);
//}

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

unsigned int veMesh::getVertexCount()
{
	return static_cast<unsigned int>(_vertices->size() * sizeof(GLfloat)) / getVertexStride();
}

void veMesh::addVertexAtrribute(const VertexAtrribute &attri)
{
	veAssert(_attributes.size() < MAX_ATTRIBUTE_NUM);
	_attributes.push_back(attri);
	_needRefresh = true;
    static_cast<veMeshRenderer *>(_renderer.get())->needRefresh();
}

bool veMesh::getVertexAtrribute(VertexAtrribute::AtrributeType aType, VertexAtrribute &attri) const
{
	for (auto &iter : _attributes) {
		if (iter.attributeType == aType) {
			attri = iter;
			return true;
		}
	}
	return false;
}

bool veMesh::getVertexAtrributeSizeInByte(VertexAtrribute::AtrributeType aType, unsigned int &sizeInByte) const
{
	for (auto &iter : _attributes) {
		if (iter.attributeType == aType) {
			if (iter.valueType == veMesh::VertexAtrribute::FLOAT) sizeInByte = sizeof(GLfloat) * iter.valueNum;
			else if (iter.valueType == veMesh::VertexAtrribute::UINT) sizeInByte = sizeof(GLuint) * iter.valueNum;
			else if (iter.valueType == veMesh::VertexAtrribute::USHORT) sizeInByte = sizeof(GLushort) * iter.valueNum;
			return true;
		}
	}
	return false;
}

void veMesh::addPrimitive(const Primitive &primitive)
{
	_primitives.push_back(primitive);
	_needRefresh = true;
    static_cast<veMeshRenderer *>(_renderer.get())->needRefresh();
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
    static_cast<veMeshRenderer *>(_renderer.get())->needRefresh();
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

void veMesh::generateTransformFeedbackBuffer(unsigned int contextID)
{
	if (!_bones.empty()) {
		if (!_transformFeedbackBuffer.valid()) {
			_transformFeedbackBuffer = veGLDataBufferManager::instance()->createGLDataBuffer([]() -> GLuint{
				GLuint vbo;
				glGenBuffers(1, &vbo);
				return vbo;
			}, [](GLuint vbo){
				glDeleteBuffers(1, &vbo);
			});
		}

		auto tfb = _transformFeedbackBuffer->getData(contextID);
		if (_needRefresh || !tfb) {
			unsigned int stride = (3 + 3) * sizeof(GLfloat);
			unsigned int bufSize = stride * getVertexCount();
			if (!tfb) {
				tfb = _transformFeedbackBuffer->createData(contextID);
			}
			glBindBuffer(GL_ARRAY_BUFFER, tfb);
			glBufferData(GL_ARRAY_BUFFER, bufSize, nullptr, GL_DYNAMIC_COPY);
			_transformFeedbackBufferSize = bufSize;
		}
		glBindBuffer(GL_ARRAY_BUFFER, tfb);
		_transformFeedbackVertices = (veReal *)glMapBufferRange(GL_ARRAY_BUFFER, 0, _transformFeedbackBufferSize, GL_MAP_READ_BIT);
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void veMesh::traversePrimitives(const PrimitiveCallback &callback)
{
	for (auto &primitive : _primitives) {
		if (primitive.primitiveType == Primitive::TRIANGLES) {
			if (!_bones.empty()) {
				if (!_transformFeedbackVertices) {
					veLog("GL_TRANSFORM_FEEDBACK_BUFFER is NULL!");
					return;
				}
				unsigned int stride = (3 + 3);
				unsigned int vOffset = 0;
				unsigned int nOffset = 3;
				for (size_t idx = 0; idx < primitive.indices->size(); idx += 3) {
					unsigned int id0 = (*primitive.indices)[idx];
					unsigned int id1 = (*primitive.indices)[idx + 1];
					unsigned int id2 = (*primitive.indices)[idx + 2];
					veReal *p1 = &(_transformFeedbackVertices)[id0 * stride + vOffset];
					veReal *p2 = &(_transformFeedbackVertices)[id1 * stride + vOffset];
					veReal *p3 = &(_transformFeedbackVertices)[id2 * stride + vOffset];
					veReal *n1 = &(_transformFeedbackVertices)[id0 * stride + nOffset];
					veReal *n2 = &(_transformFeedbackVertices)[id1 * stride + nOffset];
					veReal *n3 = &(_transformFeedbackVertices)[id2 * stride + nOffset];
					if (callback != nullptr) {
						if (callback(p1, p2, p3, n1, n2, n3))
							break;
					}
				}
			}
			else {
				unsigned int stride = 0;
				unsigned int vOffset = 0;
				unsigned int vCount = 0;
				unsigned int nOffset = 0;
				unsigned int nCount = 0;
				for (auto iter : _attributes) {
					if (iter.attributeType == VertexAtrribute::VERTEX_ATTRIB_POSITION) {
						vOffset = stride;
						vCount = iter.valueNum;
					}
					else if (iter.attributeType == VertexAtrribute::VERTEX_ATTRIB_NORMAL) {
						nOffset = stride;
						nCount = iter.valueNum;
					}
					if (iter.valueType == veMesh::VertexAtrribute::FLOAT) stride += iter.valueNum;
					else if (iter.valueType == veMesh::VertexAtrribute::UINT) stride += sizeof(GLuint) * iter.valueNum / sizeof(GLfloat);
					else if (iter.valueType == veMesh::VertexAtrribute::USHORT) stride += sizeof(GLushort) * iter.valueNum / sizeof(GLfloat);
				}

				if (vCount < 3 || nCount < 3) {
					veLog("Vertex or Normal Size < 3, not support in traversePrimitive function.");
					return;
				}
				for (size_t idx = 0; idx < primitive.indices->size(); idx += 3) {
					unsigned int id0 = (*primitive.indices)[idx];
					unsigned int id1 = (*primitive.indices)[idx + 1];
					unsigned int id2 = (*primitive.indices)[idx + 2];
					veReal *p1 = &(*_vertices)[id0 * stride + vOffset];
					veReal *p2 = &(*_vertices)[id1 * stride + vOffset];
					veReal *p3 = &(*_vertices)[id2 * stride + vOffset];
					veReal *n1 = &(*_vertices)[id0 * stride + nOffset];
					veReal *n2 = &(*_vertices)[id1 * stride + nOffset];
					veReal *n3 = &(*_vertices)[id2 * stride + nOffset];
					if (callback != nullptr) {
						if (callback(p1, p2, p3, n1, n2, n3))
							break;
					}
				}
			}
		}
	}

}

void veMesh::updateBoundingBox()
{
	if (_needRefresh) {
		caculateBoundingBox();
	}
	if (!_bones.empty()) {
		_boundingBox.dirty();
        veMat4 toNodeMatrix = _parents.empty()? veMat4::IDENTITY: _parents[0]->getWorldToNodeMatrix();
		for (auto &iter : _bones) {
			veMat4 boneMat = toNodeMatrix * iter->getBoneNode()->getNodeToWorldMatrix() * iter->getOffsetMat();
			iter->setBoundingBox(iter->getBindPosBoundingBox() * boneMat);
			_boundingBox.expandBy(iter->getBoundingBox());
		}
	}
}

bool veMesh::intersectWith(veRay *ray, veVec3 &position, veVec3 &normal)
{
	bool state = false;
	traversePrimitives([&](const veReal *p1, const veReal *p2, const veReal *p3
		, const veReal *n1, const veReal *n2, const veReal *n3) -> bool {
		if (ray->isIntersectWith(veVec3(p1[0], p1[1], p1[2]), veVec3(p2[0], p2[1], p2[2]), veVec3(p3[0], p3[1], p3[2]), &position, &normal)) {
			state = true;
			return true;
		}
		return false;
	});
	return state;
}
