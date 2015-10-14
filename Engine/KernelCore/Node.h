#ifndef _VE_NODE_
#define _VE_NODE_
#include "Prerequisites.h"
#include "BaseCore/Matrix4.h"
#include "VE_Ptr.h"
#include "Event.h"
#include "Component.h"
#include "RenderableObject.h"

class veVisualiser;
class veSceneManager;
class veNodeVisitor;
class VE_EXPORT veNode
{
public:

	typedef std::vector< VE_Ptr<veComponent> >        Components;
	typedef std::vector< VE_Ptr<veNode> >             Children;
	typedef std::vector< VE_Ptr<veRenderableObject> > RenderableObjects;

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	virtual ~veNode();

	void setVisible(bool isVis) { _isVisible = isVis; }
	bool isVisible() const { return _isVisible; };

	int addChild(veNode *child);
	veNode* createChild();
	bool removeChild(veNode *child);
	veNode* removeChild(unsigned int cIndex);
	veNode* getChild(unsigned int cIndex);
	size_t getChildCount() const { return _children.size(); }

	veNode* getParent() { return _parent; }

	int addComponent(veComponent *com);
	bool removeComponent(veComponent *com);
	veComponent* removeComponent(unsigned int comIndex);
	veComponent* getComponent(unsigned int comIndex);
	size_t getComponentCount() const { return _components.size(); }

	int addRenderableObject(veRenderableObject *obj);
	bool removeRenderableObject(veRenderableObject *obj);
	veRenderableObject* removeRenderableObject(size_t objIndex);
	veRenderableObject* getRenderableObject(size_t objIndex);
	size_t getRenderableObjectCount() const { return _renderableObjects.size(); }

	virtual void setMatrix(const veMat4 &mat) { _matrix = mat; _refresh = true; }
	const veMat4& getMatrix() const { return _matrix; }

	void setBoundingBox(const veBoundingBox &bbox) { _boundingBox = bbox; }
	const veBoundingBox& getBoundingBox() const { return _boundingBox; }
	void setAutoUpdateBoundingBox(bool isAuto) { _autoUpdateBoundingBox = isAuto; }

	void setSceneManager(veSceneManager *sm) { _sceneManager = sm; }
	veSceneManager* getSceneManager() { return _sceneManager; }

	veMat4 getNodeToWorldMatrix();
	veMat4 getWorldToNodeMatrix();

	void setMask(unsigned int mask, bool isOverride = false) { _mask = mask; _overrideMask = isOverride; }
	unsigned int getMask() const { return _mask; }

	void refresh();

	virtual bool routeEvent(const veEvent &event, veVisualiser *vs);
	virtual void update(veSceneManager *sm);
	virtual void render(veCamera *camera);

	virtual void accept(veNodeVisitor &visitor);
	virtual void visit(veNodeVisitor &visitor);

protected:

	veNode();
	void updateBoundingBox();

protected:

	Components        _components;
	Children          _children;
	RenderableObjects _renderableObjects;
	veNode           *_parent;

	veBoundingBox     _boundingBox;
	veMat4            _matrix;
	bool              _isVisible;
	bool              _refresh;
	unsigned int      _mask;
	bool              _overrideMask;
	bool              _autoUpdateBoundingBox;

	veSceneManager   *_sceneManager;
};

#endif