#ifndef _VE_DEBUGER_
#define _VE_DEBUGER_
#include "Prerequisites.h"
#include "Component.h"

class veRenderableObject;
class VE_EXPORT veDebuger : public veComponent
{
public:
    veDebuger();
    ~veDebuger();
    
    virtual void update(veNode *node, veVisualiser *vs) override;
    virtual void render(veCamera *camera) override;
    
protected:
    
    std::vector<veRenderableObject *> _renderableObjects;
};

#endif /* Debuger_h */
