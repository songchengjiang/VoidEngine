#ifndef _VE_NODE_
#define _VE_NODE_
#include "Prerequisites.h"
#include "BaseCore/Matrix4.h"
#include "VE_Ptr.h"
#include "Event.h"
#include "Inputer.h"
#include "Component.h"

class veVisualiser;
class VE_EXPORT veNode
{
public:

	typedef std::vector< VE_Ptr<veComponent> > Components;
	typedef std::vector< VE_Ptr<veNode> >      Children;

	veNode();
	virtual ~veNode();

	int addChild(veNode *child);
	bool removeChild(veNode *child);
	veNode* removeChild(unsigned int cIndex);
	veNode* getChild(unsigned int cIndex);
	unsigned int getChildCount() const { return _children.size(); }

	int addComponent(veComponent *com);
	bool removeComponent(veComponent *com);
	veComponent* removeComponent(unsigned int comIndex);
	veComponent* getComponent(unsigned int comIndex);
	unsigned int getComponentCount() const { return _components.size(); }

	void setMatrix(const veMat4 &mat) { _matrix = mat; }
	const veMat4& getMatrix() const { return _matrix; }

	bool routeEvent(double deltaTime, const veEvent &event, veVisualiser *vs);
	void update(double deltaTime, veVisualiser *vs);

protected:

	Components     _components;
	Children       _children;

	veMat4         _matrix;
};

#endif