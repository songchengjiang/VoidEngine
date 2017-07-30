#include "Node.h"
#include "NodeVisitor.h"
#include "SceneManager.h"
#include <algorithm>

veNode::veNode()
	: USE_VE_PTR_INIT
	, _parent(nullptr)
	, _matrix(veMat4::IDENTITY)
	, _worldMatrix(veMat4::IDENTITY)
	, _isVisible(true)
	, _isDynamic(true)
	, _refresh(true)
	, _mask(0xffffffff)
	, _overrideMask(false)
	, _autoUpdateBoundingBox(true)
	, _sceneManager(nullptr)
	, _isInScene(false)
	, _userData(nullptr)
{
}

veNode::~veNode()
{
    for (auto &com : _components){
        com->onDetachToNode(this);
    }
    
    for (auto &rb : _renderableObjects){
        auto parent = std::find(rb->_parents.begin(), rb->_parents.end(), this);
        if (parent != rb->_parents.end()) {
            rb->_parents.erase(parent);
        }
    }
}

int veNode::addChild(veNode *child)
{
	if (!child) return -1;
	auto iter = std::find(_children.begin(), _children.end(), child);
	if (iter != _children.end()) return -1;
	veAssert(child->_parent == nullptr);
	child->_parent = this;
	_children.push_back(child);
    child->setVisible(_isVisible);
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

veNode* veNode::removeChild(size_t cIndex)
{
	veAssert(cIndex < _children.size());
	veNode* child = _children[cIndex].get();
	child->_parent = nullptr;
	child->_isInScene = false;
	_children.erase(_children.begin() + cIndex);
	return child;
}

veNode* veNode::getChild(size_t cIndex)
{
	veAssert(cIndex < _children.size());
	return _children[cIndex].get();
}

veNode* veNode::findChildBy(const std::string &name)
{
	class findNodeVistor : public veNodeVisitor {
	public:
		findNodeVistor(const std::string &nm) 
			: _name(nm)
			, node(nullptr){
		
		}
		virtual bool visit(veNode &node) override {
			if (node.getName() == _name) {
				this->node = &node;
				return true;
			}
			return false;
		}

		veNode *node;

	private:
		std::string _name;
	};

	findNodeVistor nv(name);
	this->accept(nv);
	return nv.node;
}

int veNode::addComponent(veComponent *com)
{
    if (!com) return -1;
	auto iter = std::find(_components.begin(), _components.end(), com);
	if (iter != _components.end()) return -1;
	if (!com->onAttachToNode(this)) return -1;
	_components.push_back(com);
	return int(_components.size() - 1);
}

bool veNode::removeComponent(veComponent *com)
{
    if (!com) return false;
	auto iter = std::find(_components.begin(), _components.end(), com);
	if (iter == _components.end()) return false;
	if (!com->onDetachToNode(this)) return false;
	_components.erase(iter);
	return true;
}

veComponent* veNode::removeComponent(size_t comIndex)
{
	veAssert(comIndex < _components.size());
	veComponent* com = _components[comIndex].get();
	_components.erase(_components.begin() + comIndex);
	com->onDetachToNode(this);
	return com;
}

veComponent* veNode::getComponent(size_t comIndex)
{
	veAssert(comIndex < _components.size());
	return _components[comIndex].get();
}

int veNode::addRenderableObject(veRenderableObject *obj)
{
	if (!obj) return -1;
	auto iter = std::find(_renderableObjects.begin(), _renderableObjects.end(), obj);
	if (iter != _renderableObjects.end()) return -1;
	obj->_parents.push_back(this);
	_renderableObjects.push_back(obj);
	return int(_renderableObjects.size() - 1);
}

bool veNode::removeRenderableObject(veRenderableObject *obj)
{
	auto iter = std::find(_renderableObjects.begin(), _renderableObjects.end(), obj);
	if (iter == _renderableObjects.end()) return false;
	auto parent = std::find(iter->get()->_parents.begin(), iter->get()->_parents.end(), this);
	if (parent != iter->get()->_parents.end()) {
		iter->get()->_parents.erase(parent);
	}
	_renderableObjects.erase(iter);
	return true;
}

veRenderableObject* veNode::removeRenderableObject(size_t objIndex)
{
	veAssert(objIndex < _renderableObjects.size());
	veRenderableObject* obj = _renderableObjects[objIndex].get();
	auto parent = std::find(obj->_parents.begin(), obj->_parents.end(), this);
	if (parent != obj->_parents.end()) {
		obj->_parents.erase(parent);
	}
	_renderableObjects.erase(_renderableObjects.begin() + objIndex);
	return obj;
}

veRenderableObject* veNode::getRenderableObject(size_t objIndex)
{
	veAssert(objIndex < _renderableObjects.size());
	return _renderableObjects[objIndex].get();
}

const veMat4& veNode::getNodeToWorldMatrix() const
{
	if (_refresh) {
		_worldMatrix = _parent == nullptr ? _matrix : _parent->getNodeToWorldMatrix() * _matrix;
		return _worldMatrix;
	}else
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

void veNode::setVisible(bool isVis)
{
    if (_isVisible == isVis)
        return;
    _isVisible = isVis;
    if (!_children.empty()){
        for (auto &child : _children){
            child->setVisible(_isVisible);
        }
    }
}

void veNode::setMask(unsigned int mask, bool isOverride /*= true*/)
{
	_mask = mask; 
	_overrideMask = isOverride; 
	//_sceneManager->needReload();
}

void veNode::refresh()
{
	_refresh = true;
}

bool veNode::routeEvent(veSceneManager *sm, veViewer *viewer, const veEvent &event)
{
	if (!_isVisible) return false;

    for (auto &com : _components) {
        if (com->isEnabled()){
            if (event.getEventType() & com->getEventFilter()) {
                if (com->handle(sm, viewer, event))
                    return true;
            }
        }
    }
    
	if (!_children.empty()){
		for (auto &child : _children){
			if (child->routeEvent(sm, viewer, event)) return true;
		}
	}

	if (!_renderableObjects.empty()) {
		for (auto &iter : _renderableObjects) {
			if (iter->handle(this, sm, viewer, event)) return true;
		}
	}

	return false;
}

void veNode::update(veSceneManager *sm, const veMat4 &transform)
{
    if (_isVisible) {
        if (!_isInScene) {
            if (!_components.empty()) {
                for (auto &com : _components) {
                    if (com->isEnabled())
                        com->start(sm);
                }
            }
            _isInScene = true;
        }
        
        if (!_components.empty()) {
            for (auto &com : _components) {
                if (com->isEnabled())
                    com->update(sm);
            }
        }
        
        if (_parent && _parent->_overrideMask) {
            _mask = _parent->getMask();
            _overrideMask = true;
        }
        
        veMat4 worldMat = this->getWorldToNodeMatrix();
        
        if (!_children.empty()) {
            for (auto &child : _children) {
                if (_refresh) child->refresh();
                child->update(sm, worldMat);
            }
        }
        
        if (!_renderableObjects.empty()) {
            for (auto &iter : _renderableObjects) {
                iter->update(this, sm);
            }
        }
        
        updateBoundingBox();
        _refresh = false;
        _overrideMask = false;
    }
	updateSceneManager();
}

void veNode::beforeRender(veSceneManager *sm, veViewer *viewer)
{
    if (!_isVisible) return;
    if (!_components.empty()) {
        for (auto &com : _components) {
            if (com->isEnabled())
                com->beforeRender(sm, viewer);
        }
    }
    
    if (!_children.empty()) {
        for (auto &child : _children) {
            child->beforeRender(sm, viewer);
        }
    }
}

void veNode::afterRender(veSceneManager *sm, veViewer *viewer)
{
    if (!_isVisible) return;
    if (!_components.empty()) {
        for (auto &com : _components) {
            if (com->isEnabled())
                com->afterRender(sm, viewer);
        }
    }
    
    if (!_children.empty()) {
        for (auto &child : _children) {
            child->afterRender(sm, viewer);
        }
    }
}

void veNode::accept(veNodeVisitor &visitor)
{
	if (visit(visitor))
		return;
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

bool veNode::visit(veNodeVisitor &visitor)
{
	return visitor.visit(*this);
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
                if (!iter->getBoundingBox().isNull()) {
                    if (iter->isLocalBoundingBox())
                        _boundingBox.expandBy(iter->getBoundingBox() * _worldMatrix);
                    else
                        _boundingBox.expandBy(iter->getBoundingBox());
                }
			}
		}
	}
}

void veNode::updateSceneManager()
{
    _sceneManager->requestRender(this);
}
