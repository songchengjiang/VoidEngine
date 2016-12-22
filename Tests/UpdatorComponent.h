#ifndef _UPDATOR_COMPONENT_
#define _UPDATOR_COMPONENT_
#include "VoidEngine.h"
#include <functional>

class UpdatorComponent : public veComponent
{
public:
    UpdatorComponent();
    ~UpdatorComponent();
    
    virtual void update(veSceneManager *sm) override;
    
    std::function<void(veSceneManager *sm, veNode *node)> UpdateCallback;
};


#endif
