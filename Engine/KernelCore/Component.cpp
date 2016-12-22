#include "Component.h"
#include "Node.h"
#include "SceneManager.h"
#include <algorithm>

veComponent::veComponent()
	: USE_VE_PTR_INIT
	, _filter(veEvent::VE_ALL_EVENT)
    , _isEnabled(true)
    , _attachedNode(nullptr)
{
}

veComponent::~veComponent()
{

}

bool veComponent::onAttachToNode(veNode *node)
{
    if (_attachedNode != nullptr)
        return false;
    _attachedNode = node;
    this->awake();
    return true;
}

bool veComponent::onDetachToNode(veNode *node)
{
    if (_attachedNode == nullptr)
        return false;
    this->asleep();
    _attachedNode = nullptr;
    return true;
}
