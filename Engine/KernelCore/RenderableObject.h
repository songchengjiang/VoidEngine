#ifndef _VE_RENDERABLE_OBJECT_
#define _VE_RENDERABLE_OBJECT_
#include "Prerequisites.h"
#include "Renderer.h"
#include "Material.h"
#include "BoudingBox.h"

class veRay;
class veNode;
class veVisualiser;
class veSceneManager;
class veViewer;
class VE_EXPORT veRenderableObject
{
	friend class veNode;
	friend class veSceneManager;
public:

	veRenderableObject(veSceneManager *sm);
	virtual ~veRenderableObject();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	virtual bool handle(veNode *node, veSceneManager *sm, veViewer *viewer, const veEvent &event) { return false; }
	virtual void update(veNode *node, veSceneManager *sm);
	virtual void render(veNode *node, veCamera *camera, unsigned int contextID);
	virtual bool intersectWith(veRay *ray, veNode *node) { return false; };

	void setVisible(bool isVis) { _isVisible = isVis; }
	bool isVisible() const { return _isVisible; };
	virtual void setRenderer(veRenderer *renderer) { _renderer = renderer; }
	veRenderer* getRenderer() const { return _renderer.get(); }
	virtual void setMaterial(veMaterial *material) { _material = material; }
	veMaterial* getMaterial() { return _material.get(); }
	const veMaterial* getMaterial() const { return _material.get(); }
	void setBoundingBox(const veBoundingBox &bbox) { _boundingBox = bbox; }
	const veBoundingBox& getBoundingBox() const { return _boundingBox; }
    bool isLocalBoundingBox() const { return _isLocalBoundingBox; }

	const std::vector<veNode *>& getParents() const { return _parents; }

	//void dirtyBoundingBox() { _isDirtyBoundingBox = true; }

protected:

	VE_Ptr<veRenderer> _renderer;
	VE_Ptr<veMaterial> _material;
	std::vector<veNode *> _parents;
	veBoundingBox      _boundingBox;
    bool               _isLocalBoundingBox;
	bool              _isVisible;
	bool              _isDirtyBoundingBox;
	bool              _isInScene;
	veSceneManager   *_sceneManager;
};

#endif