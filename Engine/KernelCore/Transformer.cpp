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
		for (auto &node : _attachedNodeList) {
			veMat4 m;
			m.makeTransform(_position, _scale, _rotation);
			node->setMatrix(m);
		}
		_needUpdate = false;
	}
}

void veTransformer::onAttachToNode(veNode *node)
{
	veComponent::onAttachToNode(node);
}

void veTransformer::onDetachToNode(veNode *node)
{
	veComponent::onDetachToNode(node);
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