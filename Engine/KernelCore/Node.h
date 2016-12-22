#ifndef _VE_NODE_
#define _VE_NODE_
#include "Prerequisites.h"
#include "BaseCore/Matrix4.h"
#include "VE_Ptr.h"
#include "Event.h"
#include "Component.h"
#include "RenderableObject.h"
#include <functional>

class veViewer;
class veSceneManager;
class veNodeVisitor;

class VE_EXPORT veNode
{
public:

	typedef std::vector< VE_Ptr<veNode> >             Children;
	typedef std::vector< VE_Ptr<veRenderableObject> > RenderableObjects;

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	virtual ~veNode();

	void setVisible(bool isVis) { _isVisible = isVis; }
	bool isVisible() const { return _isVisible; };
	void setDynamicNode(bool isStatic) { _isDynamic = isStatic; }
	bool isDynamicNode() { return _isDynamic; }

	void setMask(unsigned int mask, bool isOverride = false);
	unsigned int getMask() const { return _mask; }

	virtual int addChild(veNode *child);
	virtual bool removeChild(veNode *child);
	virtual veNode* removeChild(size_t cIndex);
	virtual veNode* getChild(size_t cIndex);
	size_t getChildCount() const { return _children.size(); }
	veNode* findChildBy(const std::string &name);

	veNode* getParent() { return _parent; }

	int addComponent(veComponent *com);
	bool removeComponent(veComponent *com);
	veComponent* removeComponent(size_t comIndex);
	veComponent* getComponent(size_t comIndex);
	size_t getComponentCount() const { return _components.size(); }

	virtual int addRenderableObject(veRenderableObject *obj);
	bool removeRenderableObject(veRenderableObject *obj);
	veRenderableObject* removeRenderableObject(size_t objIndex);
	veRenderableObject* getRenderableObject(size_t objIndex);
	size_t getRenderableObjectCount() const { return _renderableObjects.size(); }

	virtual void setMatrix(const veMat4 &mat) { _matrix = mat; refresh(); }
	const veMat4& getMatrix() const { return _matrix; }

	void setBoundingBox(const veBoundingBox &bbox) { _boundingBox = bbox; }
	const veBoundingBox& getBoundingBox() const { return _boundingBox; }
	void setAutoUpdateBoundingBox(bool isAuto) { _autoUpdateBoundingBox = isAuto; }

	void setSceneManager(veSceneManager *sm) { _sceneManager = sm; }
	veSceneManager* getSceneManager() { return _sceneManager; }

	bool isInScene() const { return _isInScene; }

	virtual veMat4 getNodeToWorldMatrix() const;
	virtual veMat4 getWorldToNodeMatrix() const;

	veMat4 computeNodeToWorldMatrix() const;
	veMat4 computeWorldToNodeMatrix() const;

	void setUserData(void *ud) { _userData = ud; }
	void* getUserData() { return _userData; }
	const void* getUserData() const { return _userData; }

	virtual void refresh();

	virtual bool routeEvent(veSceneManager *sm, veViewer *viewer, const veEvent &event);
	virtual void update(veSceneManager *sm, const veMat4 &transform);

	virtual void accept(veNodeVisitor &visitor);
	virtual bool visit(veNodeVisitor &visitor);

protected:

	veNode();
	void updateBoundingBox();
	virtual void refreshUpdate(veSceneManager *sm, const veMat4 &transform);
	virtual void updateSceneManager();

protected:

	veComponentList   _components;
	Children          _children;
	RenderableObjects _renderableObjects;
	veNode           *_parent;

	veBoundingBox     _boundingBox;
	veMat4            _matrix;
	veMat4            _worldMatrix;
	bool              _isVisible;
	bool              _refresh;
	unsigned int      _mask;
	bool              _overrideMask;
	bool              _autoUpdateBoundingBox;
	bool              _isInScene;
	bool              _isDynamic;

	void             *_userData;
	veSceneManager   *_sceneManager;
};

typedef std::vector< VE_Ptr<veNode> > veNodeList;

#endif
