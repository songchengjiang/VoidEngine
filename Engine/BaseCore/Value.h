#ifndef _VE_VALUE_
#define _VE_VALUE_
#include "Prerequisites.h"
#include "Vector2.h"
#include "Spline.h"

class VE_EXPORT veValue
{
public:
    
    USE_VE_PTR;
    
    enum class Type{
        VAL_FIXED,
        VAL_RANDOM,
        VAL_CURVED,
        VAL_OSCILLATE,
    };
    
    virtual ~veValue();
    
    virtual veReal getValue(veReal x = 0.0f) = 0;
    Type getType() const { return _type; }
    
    
protected:
    
    veValue(Type type);
    
protected:
    
    Type _type;
};

class VE_EXPORT veValueFixed : public veValue
{
public:
    
    veValueFixed();
    virtual ~veValueFixed();
    
    virtual veReal getValue(veReal x = 0.0f) override;
    
    void setValue(veReal val) { _value = val; }
    
private:
    
    veReal _value;
};

class VE_EXPORT veValueRandom : public veValue
{
public:
    
    veValueRandom();
    virtual ~veValueRandom();
    
    virtual veReal getValue(veReal x = 0.0f) override;

    void setMinimum(veReal min) { _min = min; }
    veReal getMinimum() const { return _min; }
    void setMaximum(veReal max) { _max = max; }
    veReal getMaximum() const { return _max; }
    
private:
    
    veReal _min;
    veReal _max;
};

class VE_EXPORT veValueCurved : public veValue
{
public:
    typedef std::vector<veVec2> ControlPointList;
    
    enum class InterpolationType{
        IT_LINEAR,
        IT_SPLINE,
    };
    
    veValueCurved();
    virtual ~veValueCurved();
    
    virtual veReal getValue(veReal x = 0.0f) override;
    
    void setInterpolationType(InterpolationType type) { _iType = type; }
    InterpolationType getInterpolationType() const { return _iType; }
    
    void addControlPoint(veReal x, veReal y);
    const ControlPointList& getControlPoints() const { return _cpList; }
    void processControlPoints();
    size_t getNumControlPoints() const { return _cpList.size(); }
    void removeAllControlPoints();
    
private:
    
    inline ControlPointList::iterator findNearestControlPointIterator(float x);
    inline ControlPointList::iterator getFirstValidIterator();
    inline ControlPointList::iterator getLastValidIterator();
    
private:
    
    float             _range;
    InterpolationType _iType;
    ControlPointList  _cpList;
    veSpline          _spline;
};

class VE_EXPORT veValueOscillate : public veValue
{
public:
    
    enum class OscillationType{
        OSCT_SINE,
        OSCT_SQUARE,
    };
    
    veValueOscillate();
    virtual ~veValueOscillate();
    
    virtual veReal getValue(veReal x = 0.0f) override;
    
    void setOscillationType(OscillationType type) { _oscType = type; }
    OscillationType getOscillationType() const { return _oscType; }
    
    float getFrequency () const { return _frequency; }
    void setFrequency (float frequency) { _frequency = frequency; }
    
    float getPhase () const { return _phase; }
    void setPhase (float phase) { _phase = phase; }
    
    float getBase () const { return _base; }
    void setBase (float base) { _base = base; }
    
    float getAmplitude () const { return _amplitude; }
    void setAmplitude (float amplitude) { _amplitude = amplitude; }
    
private:
    
    OscillationType _oscType;
    float _frequency;
    float _phase;
    float _base;
    float _amplitude;
};

#endif
