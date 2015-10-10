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
