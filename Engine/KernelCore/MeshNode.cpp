#include "MeshNode.h"
#include "Mesh.h"

veMeshNode::veMeshNode()
	: USE_VE_PTR_INIT
	, _parent(nullptr)
{
}

veMeshNode::~veMeshNode()
{

}

int veMeshNode::addChild(veMeshNode *child)
{
	if (!child) return -1;
	auto iter = std::find(_children.begin(), _children.end(), child);
	if (iter != _children.end()) return -1;
	veAssert(child->_parent == nullptr);
	child->_parent = this;
	_children.push_back(child);
	return int(_children.size() - 1);
}

bool veMeshNode::removeChild(veMeshNode *child)
{
	auto iter = std::find(_children.begin(), _children.end(), child);
	if (iter == _children.end()) return false;
	(*iter)->_parent = nullptr;
	_children.erase(iter);
	return true;
}

veMeshNode* veMeshNode::removeChild(unsigned int cIndex)
{
	veAssert(cIndex < _children.size());
	veMeshNode* child = _children[cIndex].get();
	child->_parent = nullptr;
	_children.erase(_children.begin() + cIndex);
	return child;
}

veMeshNode* veMeshNode::getChild(unsigned int cIndex)
{
	veAssert(cIndex < _children.size());
	return _children[cIndex].get();
}

void veMeshNode::setMatrix(const veMat4 &mat)
{
	_matrix = mat;
}

veMat4 veMeshNode::toMeshNodeRootMatrix()
{
	veMat4 rootMat = _matrix;
	veMeshNode *parent = _parent;
	while (parent) {
		rootMat = parent->getMatrix() * rootMat;
		parent = parent->_parent;
	}
	return rootMat;
}

void veMeshNode::addMeshRef(veMesh *mesh)
{
	_meshList.push_back(mesh);
	mesh->setAttachedNode(this);
}

bool veMeshNode::removeMeshRef(veMesh *mesh)
{
	auto iter = std::find(_meshList.begin(), _meshList.end(), mesh);
	if (iter == _meshList.end()) return false;
	(*iter)->setAttachedNode(nullptr);
	_meshList.erase(iter);
	return true;
}

veMesh* veMeshNode::removeMeshRef(unsigned int mIndex)
{
	veAssert(mIndex < _meshList.size());
	veMesh* mesh = _meshList[mIndex];
	mesh->setAttachedNode(nullptr);
	_meshList.erase(_meshList.begin() + mIndex);
	return mesh;
}
