#ifndef _VE_RENDERABLE_OBJECT_
#define _VE_RENDERABLE_OBJECT_
#include "Prerequisites.h"
#include "Renderer.h"
#include "Material.h"

class veNode;
class veVisualiser;
class VE_EXPORT veRenderableObject
{
public:

	veRenderableObject();
	virtual ~veRenderableObject();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	virtual void update(veNode *node, veVisualiser *vs);
	virtual void render(veNode *node, veCamera *camera);

	inline void setRenderer(veRenderer *renderer);
	veRenderer* getRenderer() const { return _renderer.get(); }
	void setMaterial(veMaterial *material) { _material = material; }
	veMaterial* getMaterial() { return _material.get(); }

protected:

	VE_Ptr<veRenderer> _renderer;
	VE_Ptr<veMaterial> _material;
};

#endif