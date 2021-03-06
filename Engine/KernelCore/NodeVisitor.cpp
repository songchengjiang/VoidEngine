#include "NodeVisitor.h"
#include "Node.h"
#include "Camera.h"
#include "Light.h"

veNodeVisitor::veNodeVisitor()
	: _traversalMode(TRAVERSE_CHILDREN)
{

}

veNodeVisitor::~veNodeVisitor()
{

}

bool veNodeVisitor::visit(veNode &node)
{
	return false;
}

bool veNodeVisitor::visit(veCamera &camera)
{
	return visit(static_cast<veNode&>(camera));
}