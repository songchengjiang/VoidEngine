#include "Component.h"
#include "Node.h"
#include "SceneManager.h"

veComponent::veComponent()
	: USE_VE_PTR_INIT
	, _filter(veEvent::VE_ALL_EVENT)
    , _isEnabled(true)
{
}

veComponent::~veComponent()
{

}

void veComponent::onAttachToNode(veNode *node)
{
	_attachedNodeList.push_back(node);
	node->getSceneManager()->addComponent(this);
}

void veComponent::onDetachToNode(veNode *node)
{
	auto nIter = std::find(_attachedNodeList.begin(), _attachedNodeList.end(), node);
	_attachedNodeList.erase(nIter);
	if (_attachedNodeList.empty()) {
		node->getSceneManager()->removeComponent(this);
	}
}
