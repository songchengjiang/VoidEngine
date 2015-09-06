#include "NodeVisitor.h"
#include "Node.h"
#include "Camera.h"

veNodeVisitor::veNodeVisitor()
	: _traversalMode(TRAVERSE_CHILDREN)
{

}

veNodeVisitor::~veNodeVisitor()
{

}

void veNodeVisitor::visit(veNode &node)
{

}

void veNodeVisitor::visit(veCamera &camera)
{
	visit(static_cast<veNode&>(camera));
}
