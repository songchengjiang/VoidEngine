#include "Component.h"
#include "Node.h"
#include "SceneManager.h"
#include <algorithm>

veComponent::veComponent()
	: USE_VE_PTR_INIT
	, _filter(veEvent::VE_ALL_EVENT)
    , _isEnabled(true)
    , _updateOrder(0)
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
    this->created();
    return true;
}

bool veComponent::onDetachToNode(veNode *node)
{
    if (_attachedNode == nullptr)
        return false;
    this->destroyed();
    _attachedNode = nullptr;
    return true;
}
