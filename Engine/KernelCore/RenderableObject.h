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
class VE_EXPORT veRenderableObject
{
	friend class veNode;
	friend class veSceneManager;
public:

	veRenderableObject();
	virtual ~veRenderableObject();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	virtual bool handle(veNode *node, veSceneManager *sm, const veEvent &event) { return false; }
	virtual void update(veNode *node, veSceneManager *sm);
	virtual void render(veNode *node, veCamera *camera);
	virtual bool intersectWith(veRay *ray, veNode *node) { return false; };

	void setVisible(bool isVis) { _isVisible = isVis; }
	bool isVisible() const { return _isVisible; };
	void setRenderer(veRenderer *renderer) { _renderer = renderer; }
	veRenderer* getRenderer() const { return _renderer.get(); }
	virtual void setMaterialArray(veMaterialArray *material) { _materials = material; }
	veMaterialArray* getMaterialArray() { return _materials.get(); }
	const veMaterialArray* getMaterialArray() const { return _materials.get(); }
	void setBoundingBox(const veBoundingBox &bbox) { _boundingBox = bbox; }
	const veBoundingBox& getBoundingBox() const { return _boundingBox; }

	const std::vector<veNode *> getParents() const { return _parents; }

	void dirtyBoundingBox() { _isDirtyBoundingBox = true; }

protected:

	VE_Ptr<veRenderer> _renderer;
	VE_Ptr<veMaterialArray> _materials;
	std::vector<veNode *> _parents;
	veBoundingBox      _boundingBox;
	bool              _isVisible;
	bool              _isDirtyBoundingBox;
	veSceneManager   *_sceneManager;
};

#endif