#include "Node.h"
#include "Visualiser.h"

veNode::veNode()
{

}

veNode::~veNode()
{

}

int veNode::addChild(veNode *child)
{
	auto iter = std::find(_children.begin(), _children.end(), child);
	if (iter != _children.end()) return -1;
	_children.push_back(child);
	return _children.size() - 1;
}

bool veNode::removeChild(veNode *child)
{
	auto iter = std::find(_children.begin(), _children.end(), child);
	if (iter == _children.end()) return false;
	_children.erase(iter);
	return true;
}

veNode* veNode::removeChild(unsigned int cIndex)
{
	veAssert(cIndex < _children.size());
	veNode* child = _children[cIndex].get();
	_children.erase(_children.begin() + cIndex);
	return child;
}

veNode* veNode::getChild(unsigned int cIndex)
{
	veAssert(cIndex < _children.size());
	return _children[cIndex].get();
}

int veNode::addComponent(veComponent *com)
{
	auto iter = std::find(_components.begin(), _components.end(), com);
	if (iter != _components.end()) return -1;
	_components.push_back(com);
	return _components.size() - 1;
}

bool veNode::removeComponent(veComponent *com)
{
	auto iter = std::find(_components.begin(), _components.end(), com);
	if (iter == _components.end()) return false;
	_components.erase(iter);
	return true;
}

veComponent* veNode::removeComponent(unsigned int comIndex)
{
	veAssert(comIndex < _components.size());
	veComponent* com = _components[comIndex].get();
	_components.erase(_components.begin() + comIndex);
	return com;
}

veComponent* veNode::getComponent(unsigned int comIndex)
{
	veAssert(comIndex < _components.size());
	return _components[comIndex].get();
}

bool veNode::routeEvent(double deltaTime, const veEvent &event, veVisualiser *vs)
{
	if (!_components.empty()){
		for (auto &com : _components){
			if (event.getEventType() & com->getEventFilter()){
				if (com->handle(deltaTime, this, vs, event)) return true;
			}
		}
	}

	if (!_children.empty()){
		for (auto &child : _children){
			if (child->routeEvent(deltaTime, event, vs)) return true;
		}
	}

	return false;
}

void veNode::update(double deltaTime, veVisualiser *vs)
{
	if (!_components.empty()){
		for (auto &iter : _components){
			iter->update(deltaTime, this, vs);
		}
	}
}
