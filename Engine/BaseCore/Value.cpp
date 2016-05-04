#include "Value.h"
#include "MathAlgorithm.h"

veValue::veValue(Type type)
    : USE_VE_PTR_INIT
    , _type(type)
{

}

veValue::~veValue()
{

}

veValueFixed::veValueFixed()
    : veValue(Type::VAL_FIXED)
    , _value(0.0f)
{

}

veValueFixed::~veValueFixed()
{

}

veReal veValueFixed::getValue(veReal x)
{
    return _value;
}

veValueRandom::veValueRandom()
    : veValue(Type::VAL_RANDOM)
    , _min(0.0f)
    , _max(0.0f)
{

}

veValueRandom::~veValueRandom()
{

}

veReal veValueRandom::getValue(veReal x)
{
    veReal t = veMath::veRandomUnitization();
    return _min * t + _max * (1.0f - t);
}

struct ControlPointSorter
{
    inline bool operator() (const veVec2& a, const veVec2& b)
    {
        return a.x() < b.x();
    }
};

veValueCurved::veValueCurved()
    : veValue(Type::VAL_CURVED)
    , _iType(InterpolationType::IT_LINEAR)
    , _range(0.0f)
{

}

veValueCurved::~veValueCurved()
{

}

veReal veValueCurved::getValue(veReal x)
{
    switch (_iType)
    {
        case InterpolationType::IT_LINEAR:
        {
            // Search the interval in which 'x' resides and apply linear interpolation
            if (_cpList.empty())
                return 0.0f;
            
            ControlPointList::iterator it1 = findNearestControlPointIterator(x);
            ControlPointList::iterator it2 = it1 + 1;
            if (it2 != _cpList.end())
            {
                // Calculate fraction: y = y1 + ((y2 - y1) * (x - x1)/(x2 - x1))
                return (*it1).y() + (((*it2).y() - (*it1).y()) * (x - (*it1).x())/((*it2).x() - (*it1).x()));
            }
            else
            {
                return (*it1).y();
            }
        }
            break;
            
        case InterpolationType::IT_SPLINE:
        {
            // Fit using spline
            if (_spline.getNumPoints() < 1)
                return 0.0f;
            
            float fraction = x / _range;
            return (_spline.interpolate(fraction < 1.0f ? fraction : 1.0f)).y();
        }
            break;
    }
    
    return 0.0f;
}

void veValueCurved::addControlPoint(veReal x, veReal y)
{
    _cpList.push_back(veVec2(x, y));
}

void veValueCurved::processControlPoints()
{
    if (_cpList.empty())
        return;
    
    std::sort(_cpList.begin(), _cpList.end(), ControlPointSorter());
    _range = (*getLastValidIterator()).x() - (*getFirstValidIterator()).x();
    
    if (_iType == InterpolationType::IT_SPLINE)
    {
        // Add all sorted control points to the spline
        veValueCurved::ControlPointList::iterator it;
        _spline.clear();
        for (it = _cpList.begin(); it != _cpList.end(); ++it)
            _spline.addPoint(veVec3((*it).x(), (*it).y(), 0.0f));
    }
}

void veValueCurved::removeAllControlPoints()
{
    _cpList.clear();
}

veValueCurved::ControlPointList::iterator veValueCurved::findNearestControlPointIterator(float x)
{
    // Assume that the ControlPointList is not empty
    veValueCurved::ControlPointList::iterator it;
    veValueCurved::ControlPointList::iterator itEnd = _cpList.end();
    for (it = _cpList.begin(); it != itEnd; ++it)
    {
        if (x < (*it).x())
        {
            if (it == _cpList.begin())
                return it;
            else
                return --it;
        }
    }
    
    // If not found return the last valid iterator
    return --it;
}

veValueCurved::ControlPointList::iterator veValueCurved::getFirstValidIterator()
{
    return _cpList.begin();
}

veValueCurved::ControlPointList::iterator veValueCurved::getLastValidIterator()
{
    return _cpList.end() - 1;
}

veValueOscillate::veValueOscillate()
    : veValue(Type::VAL_OSCILLATE)
    , _frequency(1.0f)
    , _phase(0.0f)
    , _base(0.0f)
    , _amplitude(1.0f)
{

}

veValueOscillate::~veValueOscillate()
{

}

veReal veValueOscillate::getValue(veReal x)
{
    switch (_oscType)
    {
        case OscillationType::OSCT_SINE:
        {
            return _base + _amplitude * veMath::veSin(_phase + _frequency * x * veMath::PI * 2.0f);
        }
            break;
        case OscillationType::OSCT_SQUARE:
        {
            float val = veMath::veSin(_phase + _frequency * x * veMath::PI * 2.0f);
            if (veMath::veAbs(val) < 0.00001f)
                val = val >0? 1: -1;
            return _base + _amplitude * val;
        }
            break;
    }
    
    return 0;
}