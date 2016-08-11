#include "ColorAffector.h"
#include "Particle.h"

veColorAffector::veColorAffector()
    : _colorOperation(ColorOperation::CAO_SET)
{

}

veColorAffector::~veColorAffector()
{

}

void veColorAffector::update(veParticle *particle, double deltaTime)
{
    // Fast rejection
    if (_colorMap.empty())
        return;
    
    {
        veVec4 color = veVec4::WHITE;
        float timeFraction = (particle->totalTimeToLive - particle->timeToLive) / particle->totalTimeToLive;
        ColorMapIterator it1 = findNearestColorMapIterator(timeFraction);
        ColorMapIterator it2 = it1;
        it2++;
        if (it2 != _colorMap.end()){
            color = it1->second + ((it2->second - it1->second) * ((timeFraction - it1->first) / (it2->first - it1->first)));
        }else{
            color = it1->second;
        }
        
        if (_colorOperation == ColorOperation::CAO_SET){
            particle->color = color;
        }else{
            particle->color = veVec4(color.x() * particle->originalColor.x(), color.y() * particle->originalColor.y(), color.z() * particle->originalColor.z(), color.w() * particle->originalColor.w());
        }
    }
}

veColorAffector::ColorMapIterator veColorAffector::findNearestColorMapIterator(float timeFraction)
{
    ColorMapIterator it;
    for (it = _colorMap.begin(); it != _colorMap.end(); ++it){
        if (timeFraction < it->first){
            if (it == _colorMap.begin())
                return it;
            else
                return --it;
        }
    }
    return --it;
}