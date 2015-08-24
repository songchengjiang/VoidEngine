#include "Node.h"
#include "Visualiser.h"

veNode::veNode()
	: USE_VE_PTR_INIT
	, _parent(nullptr)
	, _matrix(veMat4::IDENTITY)
{
}

veNode::~veNode()
{

}

int veNode::addChild(veNode *child)
{
	auto iter = std::find(_children.begin(), _children.end(), child);
	if (iter != _children.end()) return -1;
	veAssert(child->_parent == nullptr);
	child->_parent = this;
	_children.push_back(child);
	return _children.size() - 1;
}

bool veNode::removeChild(veNode *child)
{
	auto iter = std::find(_children.begin(), _children.end(), child);
	if (iter == _children.end()) return false;
	(*iter)->_parent = nullptr;
	_children.erase(iter);
	return true;
}

veNode* veNode::removeChild(unsigned int cIndex)
{
	veAssert(cIndex < _children.size());
	veNode* child = _children[cIndex].get();
	child->_parent = nullptr;
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

int veNode::addRenderableObject(veRenderableObject *obj)
{
	auto iter = std::find(_renderableObjects.begin(), _renderableObjects.end(), obj);
	if (iter != _renderableObjects.end()) return -1;
	_renderableObjects.push_back(obj);
	return _renderableObjects.size() - 1;
}

bool veNode::removeRenderableObject(veRenderableObject *obj)
{
	auto iter = std::find(_renderableObjects.begin(), _renderableObjects.end(), obj);
	if (iter == _renderableObjects.end()) return false;
	_renderableObjects.erase(iter);
	return true;
}

veRenderableObject* veNode::removeRenderableObject(unsigned int objIndex)
{
	veAssert(objIndex < _renderableObjects.size());
	veRenderableObject* obj = _renderableObjects[objIndex].get();
	_renderableObjects.erase(_renderableObjects.begin() + objIndex);
	return obj;
}

veRenderableObject* veNode::getRenderableObject(unsigned int objIndex)
{
	veAssert(objIndex < _renderableObjects.size());
	return _renderableObjects[objIndex].get();
}

veMat4 veNode::getNodeToWorldMatrix()
{
	if (!_parent) return _matrix;
	return _parent->getNodeToWorldMatrix() * _matrix;
}

veMat4 veNode::getWorldToNodeMatrix()
{
	if (!_parent) return _matrix;
	return _matrix * _parent->getWorldToNodeMatrix();
}

bool veNode::routeEvent(const veEvent &event, veVisualiser *vs)
{
	if (!_components.empty()){
		for (auto &com : _components){
			if (event.getEventType() & com->getEventFilter()){
				if (com->handle(this, vs, event)) return true;
			}
		}
	}

	if (!_children.empty()){
		for (auto &child : _children){
			if (child->routeEvent(event, vs)) return true;
		}
	}

	return false;
}

void veNode::update(veVisualiser *vs)
{
	if (!_components.empty()){
		for (auto &iter : _components){
			iter->update(this, vs);
		}
	}

	if (!_children.empty()){
		for (auto &child : _children){
			child->update(vs);
		}
	}

	if (!_renderableObjects.empty()){
		for (auto &iter : _renderableObjects){
			iter->update(this, vs);
		}
	}
}
