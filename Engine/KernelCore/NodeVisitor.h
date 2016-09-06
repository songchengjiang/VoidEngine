#ifndef _VE_NODE_VISITOR_
#define _VE_NODE_VISITOR_
#include "Prerequisites.h"

class veNode;
class veCamera;
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

	virtual bool visit(veNode &node);
	virtual bool visit(veCamera &camera);

protected:

	TraversalMode _traversalMode;
};

#endif