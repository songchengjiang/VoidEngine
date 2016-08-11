#ifndef _VE_COLOR_AFFECTOR_
#define _VE_COLOR_AFFECTOR_
#include "Affector.h"
#include <map>

class VE_EXPORT veColorAffector : public veAffector
{
public:
    typedef std::map<float, veVec4> ColorMap;
    typedef std::map<float, veVec4>::iterator ColorMapIterator;
    enum class ColorOperation
    {
        CAO_MULTIPLY,
        CAO_SET
    };

    veColorAffector();
    virtual ~veColorAffector();
    
    virtual void update(veParticle *particle, double deltaTime) override;
    
    void addColor(float timeFraction, const veVec4& color) { _colorMap[timeFraction] = color; }
    const ColorMap& getTimeAndColor() const { return _colorMap; }
    void clearColorMap () { _colorMap.clear(); }
    const ColorOperation& getColorOperation () const { return _colorOperation; }
    void setColorOperation (const ColorOperation& colorOperation) { _colorOperation = colorOperation; }
    
    
protected:
    
    inline ColorMapIterator findNearestColorMapIterator(float timeFraction);
    
protected:
    
    ColorMap       _colorMap;
    ColorOperation _colorOperation;
};

#endif
