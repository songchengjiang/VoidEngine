#ifndef _VE_NODE_VISITOR_
#define _VE_NODE_VISITOR_
#include "Prerequisites.h"

class veNode;
class veCamera;
class veLight;
class VE_EXPORT veNodeVisitor
{
public:

	enum TraversalMode
	{
		TRAVERSE_NONE,
		TRAVERSE_PARENT,
		TRAVERSE_CHILDREN,
	};
	veNodeVisitor();
	~veNodeVisitor();

	TraversalMode& traversalMode() { return _traversalMode; }

	virtual void visit(veNode &node);
	virtual void visit(veCamera &camera);
	virtual void visit(veLight &light);

protected:

	TraversalMode _traversalMode;
};

#endif