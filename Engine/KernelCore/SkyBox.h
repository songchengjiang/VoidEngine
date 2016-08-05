#ifndef _VE_SKY_BOX_
#define _VE_SKY_BOX_
#include "Prerequisites.h"
#include "Material.h"

class veCamera;
class veSceneManager;
class VE_EXPORT veSkyBox
{
	friend class veSceneManager;
public:
	~veSkyBox();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	void setSize(veReal size);
	veReal getSize()const { return _size; }
	void setMask(unsigned int mask);
	unsigned int getMask() const { return _mask; }

	void setMaterial(veMaterial *material) { _material = material; }
	veMaterial* getMaterial() { return _material.get(); }
	const veMaterial* getMaterial() const { return _material.get(); }

	void render(veCamera *camera, unsigned int contextID);

protected:

	veSkyBox(veReal size);

protected:

	veReal                  _size;
	unsigned int            _mask;
	VE_Ptr<veRenderer>      _renderer;
	VE_Ptr<veMaterial>      _material;
	veSceneManager         *_sceneManager;
};

#endif
