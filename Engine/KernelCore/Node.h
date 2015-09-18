#ifndef _VE_NODE_
#define _VE_NODE_
#include "Prerequisites.h"
#include "BaseCore/Matrix4.h"
#include "VE_Ptr.h"
#include "Event.h"
#include "Component.h"
#include "RenderableObject.h"

class veVisualiser;
class veNodeVisitor;
class VE_EXPORT veNode
{
public:

	typedef std::vector< VE_Ptr<veComponent> >        Components;
	typedef std::vector< VE_Ptr<veNode> >             Children;
	typedef std::vector< VE_Ptr<veRenderableObject> > RenderableObjects;

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	veNode();
	virtual ~veNode();

	void setVisible(bool isVis) { _isVisible = isVis; }
	bool isVisible() const { return _isVisible; };

	int addChild(veNode *child);
	bool removeChild(veNode *child);
	veNode* removeChild(unsigned int cIndex);
	veNode* getChild(unsigned int cIndex);
	unsigned int getChildCount() const { return _children.size(); }

	veNode* getParent() { return _parent; }

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

	virtual void setMatrix(const veMat4 &mat) { _matrix = mat; _refresh = true; }
	const veMat4& getMatrix() const { return _matrix; }

	veMat4 getNodeToWorldMatrix();
	veMat4 getWorldToNodeMatrix();

	void setMask(unsigned int mask) { _mask = mask; }
	unsigned int getMask() const { return _mask; }

	void refresh();

	virtual bool routeEvent(const veEvent &event, veVisualiser *vs);
	virtual void update(veVisualiser *vs);
	virtual void render(veCamera *camera);

	virtual void accept(veNodeVisitor &visitor);
	virtual void visit(veNodeVisitor &visitor);

protected:

	Components        _components;
	Children          _children;
	RenderableObjects _renderableObjects;
	veNode           *_parent;

	veMat4            _matrix;
	bool              _isVisible;
	bool              _refresh;
	unsigned int      _mask;
};

#endif