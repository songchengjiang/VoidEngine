#include "Node.h"
#include "Visualiser.h"
#include "NodeVisitor.h"
#include "SceneManager.h"

veNode::veNode()
	: USE_VE_PTR_INIT
	, _parent(nullptr)
	, _matrix(veMat4::IDENTITY)
	, _worldMatrix(veMat4::IDENTITY)
	, _isVisible(true)
	, _refresh(true)
	, _mask(0xffffffff)
	, _overrideMask(false)
	, _autoUpdateBoundingBox(true)
	, _sceneManager(nullptr)
	, _isInScene(false)
{
}

veNode::~veNode()
{

}

int veNode::addChild(veNode *child)
{
	if (!child) return -1;
	auto iter = std::find(_children.begin(), _children.end(), child);
	if (iter != _children.end()) return -1;
	veAssert(child->_parent == nullptr);
	child->_parent = this;
	_children.push_back(child);
	return int(_children.size() - 1);
}

bool veNode::removeChild(veNode *child)
{
	auto iter = std::find(_children.begin(), _children.end(), child);
	if (iter == _children.end()) return false;
	(*iter)->_parent = nullptr;
	(*iter)->_isInScene = false;
	_children.erase(iter);
	return true;
}

veNode* veNode::removeChild(unsigned int cIndex)
{
	veAssert(cIndex < _children.size());
	veNode* child = _children[cIndex].get();
	child->_parent = nullptr;
	child->_isInScene = false;
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
	return int(_components.size() - 1);
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
	if (!obj) return -1;
	auto iter = std::find(_renderableObjects.begin(), _renderableObjects.end(), obj);
	if (iter != _renderableObjects.end()) return -1;
	_renderableObjects.push_back(obj);
	return int(_renderableObjects.size() - 1);
}

bool veNode::removeRenderableObject(veRenderableObject *obj)
{
	auto iter = std::find(_renderableObjects.begin(), _renderableObjects.end(), obj);
	if (iter == _renderableObjects.end()) return false;
	_renderableObjects.erase(iter);
	return true;
}

veRenderableObject* veNode::removeRenderableObject(size_t objIndex)
{
	veAssert(objIndex < _renderableObjects.size());
	veRenderableObject* obj = _renderableObjects[objIndex].get();
	_renderableObjects.erase(_renderableObjects.begin() + objIndex);
	return obj;
}

veRenderableObject* veNode::getRenderableObject(size_t objIndex)
{
	veAssert(objIndex < _renderableObjects.size());
	return _renderableObjects[objIndex].get();
}

bool veNode::isInScene()
{
	return _isInScene;
}

veMat4 veNode::getNodeToWorldMatrix() const
{
	if (_refresh)
		return computeNodeToWorldMatrix();
	else
		return _worldMatrix;
}

veMat4 veNode::getWorldToNodeMatrix() const
{
	veMat4 mat = getNodeToWorldMatrix();
	mat.inverse();
	return mat;
}

veMat4 veNode::computeNodeToWorldMatrix() const
{
	veMat4 worldMat = _matrix;
	veNode *parent = _parent;
	while (parent) {
		worldMat = parent->getMatrix() * worldMat;
		parent = parent->_parent;
	}
	return worldMat;
}

veMat4 veNode::computeWorldToNodeMatrix() const
{
	veMat4 worldMat = computeNodeToWorldMatrix();
	worldMat.inverse();
	return worldMat;
}

void veNode::refresh()
{
	_refresh = true;
}

bool veNode::routeEvent(const veEvent &event, veSceneManager *sm)
{
	_isInScene = true;
	if (!_isVisible) return false;
	if (!_components.empty()){
		for (auto &com : _components){
			if (event.getEventType() & com->getEventFilter()){
				if (com->handle(this, sm, event)) return true;
			}
		}
	}

	if (!_children.empty()){
		for (auto &child : _children){
			if (child->routeEvent(event, sm)) return true;
		}
	}

	if (!_renderableObjects.empty()) {
		for (auto &iter : _renderableObjects) {
			if (iter->handle(this, sm, event)) return true;
		}
	}

	return false;
}

void veNode::update(veSceneManager *sm, const veMat4 &transform)
{
	if (!_isVisible) return;
	if (!_components.empty()){
		for (auto &iter : _components){
			iter->update(this, sm);
		}
	}

	if (_refresh)
		_worldMatrix = transform * _matrix;
	if (!_children.empty()){
		for (auto &child : _children){
			if (_overrideMask) child->setMask(_mask);
			if (_refresh) child->refresh();
			child->update(sm, _worldMatrix);
		}
	}

	if (!_renderableObjects.empty()){
		_sceneManager->updateRenderableNode(this);
		for (auto &iter : _renderableObjects){
			iter->update(this, sm);
		}
	}

	updateBoundingBox();
	_refresh = false;
}

void veNode::render(veCamera *camera)
{
	if (!_isVisible || camera->isOutOfFrustum(this)) return;
	if (_mask & camera->getMask()) {
		if (!_children.empty()) {
			for (auto &child : _children) {
				child->render(camera);
			}
		}

		if (!_renderableObjects.empty()) {
			for (auto &iter : _renderableObjects) {
				iter->render(this, camera);
			}
		}
	}
}

void veNode::accept(veNodeVisitor &visitor)
{
	visit(visitor);
	if (visitor.traversalMode() == veNodeVisitor::TRAVERSE_CHILDREN) {
		for (auto &iter : _children) {
			iter->accept(visitor);
		}
	}
	else if (visitor.traversalMode() == veNodeVisitor::TRAVERSE_PARENT) {
		if (_parent) {
			_parent->accept(visitor);
		}
	}
}

void veNode::visit(veNodeVisitor &visitor)
{
	visitor.visit(*this);
}

void veNode::updateBoundingBox()
{
	if (_autoUpdateBoundingBox) {
		_boundingBox.dirty();
		if (!_children.empty()) {
			for (auto &child : _children) {
				if (!child->getBoundingBox().isNull())
					_boundingBox.expandBy(child->getBoundingBox());
			}
		}

		if (!_renderableObjects.empty()) {
			for (auto &iter : _renderableObjects) {
				if (!iter->getBoundingBox().isNull())
					_boundingBox.expandBy(iter->getBoundingBox() * _worldMatrix);
			}
		}
	}
}
