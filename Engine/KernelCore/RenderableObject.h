#ifndef _VE_RENDERABLE_OBJECT_
#define _VE_RENDERABLE_OBJECT_
#include "Prerequisites.h"
#include "Renderer.h"
#include "Material.h"
#include "BoudingBox.h"

class veNode;
class veVisualiser;
class veSceneManager;
class VE_EXPORT veRenderableObject
{
public:

	veRenderableObject();
	virtual ~veRenderableObject();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	virtual bool handle(veNode *node, veSceneManager *sm, const veEvent &event) { return false; }
	virtual void update(veNode *node, veSceneManager *sm);
	virtual void render(veNode *node, veCamera *camera);


	void setVisible(bool isVis) { _isVisible = isVis; }
	bool isVisible() const { return _isVisible; };
	void setMask(unsigned int mask) { _mask = mask; }
	unsigned int getMask() const { return _mask; }
	inline void setRenderer(veRenderer *renderer);
	veRenderer* getRenderer() const { return _renderer.get(); }
	virtual void setMaterial(veMaterial *material) { _material = material; }
	veMaterial* getMaterial() { return _material.get(); }
	void setBoundingBox(const veBoundingBox &bbox) { _boundingBox = bbox; }
	const veBoundingBox& getBoundingBox() const { return _boundingBox; }

protected:

	VE_Ptr<veRenderer> _renderer;
	VE_Ptr<veMaterial> _material;
	veBoundingBox      _boundingBox;
	bool              _isVisible;
	unsigned int      _mask;
};

#endif