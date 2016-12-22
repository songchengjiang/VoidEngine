#include "Component.h"
#include "Node.h"
#include "SceneManager.h"
#include <algorithm>

veComponent::veComponent()
	: USE_VE_PTR_INIT
	, _filter(veEvent::VE_ALL_EVENT)
    , _isEnabled(true)
    , _updateOrder(0)
{
}

veComponent::~veComponent()
{

}

bool veComponent::onAttachToNode(veNode *node)
{
    auto nIter = std::find(_attachedNodeList.begin(), _attachedNodeList.end(), node);
    if (nIter != _attachedNodeList.end()) return false;
	_attachedNodeList.push_back(node);
	node->getSceneManager()->addComponent(this);
    this->created();
    return true;
}

bool veComponent::onDetachToNode(veNode *node)
{
	auto nIter = std::find(_attachedNodeList.begin(), _attachedNodeList.end(), node);
    if (nIter == _attachedNodeList.end()) return false;
    this->destroyed();
	_attachedNodeList.erase(nIter);
	if (_attachedNodeList.empty()) {
		node->getSceneManager()->removeComponent(this);
	}
    return true;
}
