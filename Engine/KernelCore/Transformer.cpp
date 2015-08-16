#include "Transformer.h"
#include "BaseCore/Matrix4.h"
#include "Node.h"

veTransformer::veTransformer()
	: _neeUpdate(true)
	, _scale(veVec3::UNIT_SCALE)
{

}

veTransformer::~veTransformer()
{

}

void veTransformer::update(double deltaTime, veNode *node, veVisualiser *vs)
{
	if (_neeUpdate){
		veMat4 m;
		m.makeTransform(_position, _scale, _rotation);
		node->setMatrix(m);
		_neeUpdate = false;
	}
}

void veTransformer::setPosition(const veVec3 &pos)
{
	_position = pos;
	_neeUpdate = true;
}

const veVec3& veTransformer::getPosition() const
{
	return _position;
}

void veTransformer::setRotation(const veQuat &rot)
{
	_rotation = rot;
	_neeUpdate = true;
}

const veQuat& veTransformer::getRotation() const
{
	return _rotation;
}

void veTransformer::setScale(const veVec3 &scl)
{
	_scale = scl;
	_neeUpdate = true;
}

const veVec3& veTransformer::getScale() const
{
	return _scale;
}

void veTransformer::translate(const veVec3 &pos)
{
	_position += pos;
	_neeUpdate = true;
}

void veTransformer::rotate(const veQuat &rot)
{
	_rotation *= rot;
	_neeUpdate = true;
}

void veTransformer::scale(const veVec3 &scl)
{
	_scale *= scl;
	_neeUpdate = true;
}
