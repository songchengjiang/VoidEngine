#ifndef _VE_NODE_
#define _VE_NODE_
#include "Prerequisites.h"
#include "BaseCore/Matrix4.h"
#include "VE_Ptr.h"
#include "Event.h"
#include "Inputer.h"
#include "Component.h"
#include "RenderableObject.h"

class veVisualiser;
class VE_EXPORT veNode
{
public:

	typedef std::vector< VE_Ptr<veComponent> >        Components;
	typedef std::vector< VE_Ptr<veNode> >             Children;
	typedef std::vector< VE_Ptr<veRenderableObject> > RenderableObjects;

	veNode();
	virtual ~veNode();

	void setName(const std::string &name) { _name = name; }
	const std::string& getName() const { return _name; }

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

	int addRenderableObject(veRenderableObject *obj);
	bool removeRenderableObject(veRenderableObject *obj);
	veRenderableObject* removeRenderableObject(unsigned int objIndex);
	veRenderableObject* getRenderableObject(unsigned int objIndex);
	unsigned int getRenderableObjectCount() const { return _renderableObjects.size(); }

	void setMatrix(const veMat4 &mat) { _matrix = mat; }
	const veMat4& getMatrix() const { return _matrix; }

	veMat4 getNodeToWorldMatrix();
	veMat4 getWorldToNodeMatrix();

	bool routeEvent(const veEvent &event, veVisualiser *vs);
	void update(veVisualiser *vs);

protected:

	Components        _components;
	Children          _children;
	RenderableObjects _renderableObjects;
	veNode           *_parent;

	veMat4            _matrix;
	std::string       _name;
};

#endif