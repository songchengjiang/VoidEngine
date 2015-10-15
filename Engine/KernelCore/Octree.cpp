#include "Octree.h"

veOctree::veOctree()
	: _parent(nullptr)
{
	memset(_children, 0, sizeof(veOctree*) * 8);
}

veOctree::~veOctree()
{

}

void veOctree::addNode(veNode *node)
{
	_nodeList.push_back(node);
}

void veOctree::removeNode(veNode *node)
{
	_nodeList.erase(std::find(_nodeList.begin(), _nodeList.end(), node));
}

void veOctree::setChild(unsigned int idx, veOctree *child)
{
	if (_children[idx]) 
		_children[idx]->_parent = nullptr;
	_children[idx] = child;
	child->_parent = this;
}
