#include "Transformer.h"
#include "BaseCore/Matrix4.h"
#include "Node.h"

veTransformer::veTransformer()
	: _needUpdate(true)
	, _scale(veVec3::UNIT_SCALE)
{

}

veTransformer::~veTransformer()
{

}

void veTransformer::update(veSceneManager *sm)
{
	if (_needUpdate && !_attachedNodeList.empty()){

		veMat4 transformerMat = veMat4::translation(_transCenter);
		veMat4 transformerInverseMat = veMat4::translation(-_transCenter);
		for (auto &node : _attachedNodeList) {
			auto nodeToTransformer = transformerInverseMat * node->getNodeToWorldMatrix();
			nodeToTransformer = veMat4::transform(_position, _scale, _rotation) * nodeToTransformer;

			auto mat = transformerMat * nodeToTransformer;
			if (node->getParent()) {
				mat = node->getParent()->getWorldToNodeMatrix() * mat;
			}
			node->setMatrix(mat);
		}
		_needUpdate = false;
	}
}

void veTransformer::onAttachToNode(veNode *node)
{
	veComponent::onAttachToNode(node);
	caculateTransformCenter();
}

void veTransformer::onDetachToNode(veNode *node)
{
	veComponent::onDetachToNode(node);
	caculateTransformCenter();
}


void veTransformer::setPosition(const veVec3 &pos)
{
	_position = pos;
	_needUpdate = true;
}

const veVec3& veTransformer::getPosition() const
{
	return _position;
}

void veTransformer::setRotation(const veQuat &rot)
{
	_rotation = rot;
	_needUpdate = true;
}

const veQuat& veTransformer::getRotation() const
{
	return _rotation;
}

void veTransformer::setScale(const veVec3 &scl)
{
	_scale = scl;
	_needUpdate = true;
}

const veVec3& veTransformer::getScale() const
{
	return _scale;
}

void veTransformer::translate(const veVec3 &pos)
{
	_position += pos;
	_needUpdate = true;
}

void veTransformer::rotate(const veQuat &rot)
{
	_rotation *= rot;
	_needUpdate = true;
}

void veTransformer::scale(const veVec3 &scl)
{
	_scale *= scl;
	_needUpdate = true;
}

void veTransformer::caculateTransformCenter()
{
	_transCenter = veVec3::ZERO;
	for (auto &node : _attachedNodeList) {
		auto &mat = node->getNodeToWorldMatrix();
		veVec3 pos;
		mat.decomposition(&pos, nullptr, nullptr);
		_transCenter += pos;
	}
}