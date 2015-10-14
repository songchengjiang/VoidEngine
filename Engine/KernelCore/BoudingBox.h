#ifndef _VE_BOUNDING_BOX_
#define _VE_BOUNDING_BOX_
#include "Prerequisites.h"
#include "BaseCore/Vector3.h"
#include "BaseCore/Matrix4.h"

class VE_EXPORT veBoundingBox
{
public:
	veBoundingBox();
	veBoundingBox(const veVec3 &min, const veVec3 &max);
	~veBoundingBox();

	void expandBy(const veVec3 &point);
	void expandBy(const veBoundingBox &bbox);
	const veVec3& min() const { return _min; }
	veVec3& min() { return _min; }
	const veVec3& max() const { return _max; }
	veVec3& max() { return _max; }
	void dirty();
	bool isNull() const;

	inline bool intersects(const veBoundingBox& bbox) const
	{
		if (this->isNull() || bbox.isNull())
			return false;

		if (_max.x() < bbox._min.x())
			return false;
		if (_max.y() < bbox._min.y())
			return false;
		if (_max.z() < bbox._min.z())
			return false;

		if (_min.x() > bbox._max.x())
			return false;
		if (_min.y() > bbox._max.y())
			return false;
		if (_min.z() > bbox._max.z())
			return false;

		return true;

	}

	inline veBoundingBox operator * (const veMat4& m) const {
		veBoundingBox bbox;
		bbox.expandBy(m * _min);
		bbox.expandBy(m * veVec3(_max.x(), _min.y(), _min.z()));
		bbox.expandBy(m * veVec3(_max.x(), _max.y(), _min.z()));
		bbox.expandBy(m * veVec3(_min.x(), _max.y(), _min.z()));

		bbox.expandBy(m * _max);
		bbox.expandBy(m * veVec3(_min.x(), _max.y(), _max.z()));
		bbox.expandBy(m * veVec3(_min.x(), _min.y(), _max.z()));
		bbox.expandBy(m * veVec3(_max.x(), _min.y(), _max.z()));
		return bbox;
	}

private:

	veVec3 _min;
	veVec3 _max;
};

#endif /* BoudingBox_hpp */
