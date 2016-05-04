#include "Spline.h"

veSpline::veSpline()
    : _autoCalc(true)
{
    _coeffs.set(2.0f, -3.0f, 0.0f, 1.0f,
               -2.0f,  3.0f, 0.0f, 0.0f,
                1.0f, -2.0f, 1.0f, 0.0f,
                1.0f, -1.0f, 0.0f, 0.0f);
}

veSpline::~veSpline()
{

}

void veSpline::addPoint(const veVec3 &point)
{
    _points.push_back(point);
    if (_autoCalc){
        reCalcTangents();
    }
}

const veVec3& veSpline::getPoint(size_t index) const
{
    veAssert(index < _points.size());
    return _points[index];
}

void veSpline::updatePoint(size_t index, const veVec3 &value)
{
    veAssert(index < _points.size());
    _points[index] = value;
    if (_autoCalc){
        reCalcTangents();
    }
}

veVec3 veSpline::interpolate(float t) const
{
    float fSeg = t * (_points.size() - 1);
    size_t segIdx = (size_t)fSeg;
    t = fSeg - segIdx;
    return interpolate(segIdx, t);
}

veVec3 veSpline::interpolate(size_t fromIndex, float t) const
{
    // Bounds check
    veAssert(fromIndex < _points.size());
    
    if ((fromIndex + 1) == _points.size())
    {
        // Duff request, cannot blend to nothing
        // Just return source
        return _points[fromIndex];
        
    }
    
    // Fast special cases
    if (t == 0.0f)
    {
        return _points[fromIndex];
    }
    else if(t == 1.0f)
    {
        return _points[fromIndex + 1];
    }
    
    // Real interpolation
    // Form a vector of powers of t
    float t2, t3;
    t2 = t * t;
    t3 = t2 * t;
    veVec4 powers(t3, t2, t, 1);
    
    
    // Algorithm is ret = powers * mCoeffs * Matrix4(point1, point2, tangent1, tangent2)
    const veVec3& point1 = _points[fromIndex];
    const veVec3& point2 = _points[fromIndex+1];
    const veVec3& tan1 = _tangents[fromIndex];
    const veVec3& tan2 = _tangents[fromIndex+1];
    veMat4 pt;
    pt.set(point1.x(), point2.x(), tan1.x(), tan2.x(),
           point1.y(), point2.y(), tan1.y(), tan2.y(),
           point1.z(), point2.z(), tan1.z(), tan2.z(),
                 1.0f,       1.0f,     1.0f,    1.0f);
    
    veVec4 ret = pt * _coeffs * powers;
    
    return veVec3(ret.x(), ret.y(), ret.z());
}

void veSpline::reCalcTangents()
{
    size_t i, numPoints;
    bool isClosed;
    
    numPoints = _points.size();
    if (numPoints < 2)
    {
        // Can't do anything yet
        return;
    }
    
    // Closed or open?
    if (_points[0] == _points[numPoints-1])
    {
        isClosed = true;
    }
    else
    {
        isClosed = false;
    }
    
    _tangents.resize(numPoints);
    
    for(i = 0; i < numPoints; ++i)
    {
        if (i ==0)
        {
            // Special case start
            if (isClosed)
            {
                // Use numPoints-2 since numPoints-1 is the last point and == [0]
                _tangents[i] = 0.5 * (_points[1] - _points[numPoints-2]);
            }
            else
            {
                _tangents[i] = 0.5 * (_points[1] - _points[0]);
            }
        }
        else if (i == numPoints-1)
        {
            // Special case end
            if (isClosed)
            {
                // Use same tangent as already calculated for [0]
                _tangents[i] = _tangents[0];
            }
            else
            {
                _tangents[i] = 0.5 * (_points[i] - _points[i-1]);
            }
        }
        else
        {
            _tangents[i] = 0.5 * (_points[i+1] - _points[i-1]);
        }
        
    }

}

void veSpline::clear()
{
    _points.clear();
    _tangents.clear();
}
