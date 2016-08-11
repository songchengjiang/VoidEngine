#ifndef _VE_SPLINE_
#define _VE_SPLINE_
#include "Prerequisites.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"

class VE_EXPORT veSpline
{
public:
    veSpline();
    ~veSpline();
    
    void addPoint(const veVec3 &point);
    const veVec3& getPoint(size_t index) const;
    size_t getNumPoints() const { return _points.size(); };
    
    void clear();
    void updatePoint(size_t index, const veVec3 &value);
    veVec3 interpolate(float t) const;
    veVec3 interpolate(size_t fromIndex, float t) const;
    void setAutoCalculate(bool autoCalc) { _autoCalc = autoCalc; };
    void reCalcTangents();
    
protected:
    
    bool _autoCalc;
    std::vector<veVec3> _points;
    std::vector<veVec3> _tangents;
    veMat4              _coeffs;
};


#endif
