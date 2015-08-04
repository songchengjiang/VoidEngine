#ifndef _VE_NODE_
#define _VE_NODE_
#include "Prerequisites.h"
#include "VE_Ptr.h"
#include "Event.h"
#include "Inputer.h"

class veVisualiser;
class VE_EXPORT veNode
{
public:

	typedef std::vector< VE_Ptr<veInputer> > InputerList;
	typedef std::vector< VE_Ptr<veNode> >    Children;

	veNode();
	virtual ~veNode();

	int addChild(veNode *child);
	bool removeChild(veNode *child);
	veNode* removeChild(unsigned int cIndex);
	veNode* getChild(unsigned int cIndex);
	unsigned int getChildCount() const { return _children.size(); }

	int addInputer(veInputer *inputer);
	bool removeInputer(veInputer *inputer);
	veInputer* removeInputer(unsigned int inpIndex);
	veInputer* getInputer(unsigned int inpIndex);
	unsigned int getInputerCount() const { return _inputerList.size(); }

	bool routeEvent(const veEvent &event, const veVisualiser *vs);

protected:

	InputerList    _inputerList;
	Children       _children;
};

#endif