#ifndef _VE_RAY_
#define _VE_RAY_
#include "Prerequisites.h"
#include "BaseCore/Vector3.h"
#include "BoudingBox.h"

class veNode;
class veRenderableObject;
class veSceneManager;
class VE_EXPORT veRay
{
public:
	struct Intersection
	{
		veNode *node;
		veRenderableObject *renderable;
		veVec3 normal;
		veVec3 position;
	};
	typedef std::vector<Intersection> Intersections;
	veRay(const veVec3 &start, const veVec3 &end);
	~veRay();

	USE_VE_PTR;

	void setStart(const veVec3 &start) { _start = start; }
	void setEnd(const veVec3 &end) { _end = end; }
	const veVec3& getStart() const { return _start; }
	const veVec3& getEnd() const { return _end; }
	const Intersections& getIntersections() const { return _intersections; }
	void addIntersection(const Intersection &inters);

	void apply(veSceneManager *sm);

	bool isIntersectWith(const veBoundingBox &bbox);
	bool isIntersectWith(const veVec3 &p1, const veVec3 &p2, const veVec3 &p3, veVec3 *intersectPoint = nullptr, veVec3 *intersectNormal = nullptr);

private:
	veVec3 _start;
	veVec3 _end;
	Intersections _intersections;
};

#endif