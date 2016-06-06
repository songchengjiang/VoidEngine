#ifndef _VE_RAY_
#define _VE_RAY_
#include "Prerequisites.h"
#include "BaseCore/Vector3.h"
#include "BoudingBox.h"
#include "VE_Ptr.h"
#include <functional>

class veNode;
class veRenderableObject;
class veSceneManager;
class VE_EXPORT veRay
{
    friend class veSceneManager;
public:
	struct Intersection
	{
		veNode *node;
		veRenderableObject *renderable;
		veVec3 localNormal;
		veVec3 localPosition;
		veVec3 worldNormal;
		veVec3 worldPosition;
	};
	typedef std::vector<Intersection> Intersections;
	~veRay();

	USE_VE_PTR;

	void setStart(const veVec3 &start);
	void setEnd(const veVec3 &end);
	const veVec3& getStart() const { return _start; }
	const veVec3& getEnd() const { return _end; }
	const Intersections& getIntersections() const { return _intersections; }
	void addIntersection(const Intersection &inters);
    void setDiscardBackFace(bool isDiscard) { _isDicardBackFace = isDiscard; }
    bool isDiscardBackFace() const { return _isDicardBackFace; }

	void apply(veSceneManager *sm, bool isSortResults = true);

	bool isIntersectWith(const veBoundingBox &bbox);
	bool isIntersectWith(const veVec3 &p0, const veVec3 &p1, const veVec3 &p2, veVec3 &intersectPoint, veVec3 &intersectNormal);

private:

	veRay(const veVec3 &start, const veVec3 &end);

private:
	veVec3 _start;
	veVec3 _end;
	veVec3 _dir;
	Intersections _intersections;
    bool        _isDicardBackFace;
};

typedef std::vector< VE_Ptr<veRay> > veRayList;

#endif