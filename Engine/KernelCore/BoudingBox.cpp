#include "BoudingBox.h"

static const veVec3 VEC3_MAX = veVec3(FLT_MAX);
static const veVec3 VEC3_MIN = veVec3(-FLT_MAX);
veBoundingBox::veBoundingBox(const veVec3 &min, const veVec3 &max)
	: _min(min)
	, _max(max)
{

}

veBoundingBox::veBoundingBox()
	: _min(FLT_MAX)
	, _max(-FLT_MAX)
{

}

veBoundingBox::~veBoundingBox()
{

}

void veBoundingBox::expandBy(const veVec3 &point)
{
	if (point.x() < _min.x()) _min.x() = point.x();
	if (point.y() < _min.y()) _min.y() = point.y();
	if (point.z() < _min.z()) _min.z() = point.z();
	if (_max.x() < point.x()) _max.x() = point.x();
	if (_max.y() < point.y()) _max.y() = point.y();
	if (_max.z() < point.z()) _max.z() = point.z();
}

void veBoundingBox::expandBy(const veBoundingBox &bbox)
{
	if (bbox.min().x() < _min.x()) _min.x() = bbox.min().x();
	if (bbox.min().y() < _min.y()) _min.y() = bbox.min().y();
	if (bbox.min().z() < _min.z()) _min.z() = bbox.min().z();
	if (_max.x() < bbox.max().x()) _max.x() = bbox.max().x();
	if (_max.y() < bbox.max().y()) _max.y() = bbox.max().y();
	if (_max.z() < bbox.max().z()) _max.z() = bbox.max().z();
}

void veBoundingBox::dirty()
{
	_min = VEC3_MAX;
	_max = VEC3_MIN;
}

bool veBoundingBox::isNull() const
{
	return _min == VEC3_MAX && _max == VEC3_MIN;
}
