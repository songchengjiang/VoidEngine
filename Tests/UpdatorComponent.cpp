#include "UpdatorComponent.h"

UpdatorComponent::UpdatorComponent()
{

}

UpdatorComponent::~UpdatorComponent()
{

}

void UpdatorComponent::update(veSceneManager *sm)
{
    if (UpdateCallback != nullptr)
        UpdateCallback(sm, _attachedNode);
}
