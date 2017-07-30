#ifndef _VE_SKY_BOX_
#define _VE_SKY_BOX_
#include "Prerequisites.h"
#include "Material.h"
#include "Component.h"
#include "Mesh.h"

class veCamera;
class veSceneManager;
class VE_EXPORT veSkyBox : public veComponent
{
public:
    veSkyBox(veReal size);
	~veSkyBox();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	void setSize(veReal size);
	veReal getSize()const { return _size; }

	void setMaterial(veMaterial *material) { _material = material; }
	veMaterial* getMaterial() { return _material.get(); }
	const veMaterial* getMaterial() const { return _material.get(); }
    
    virtual void beforeRender(veSceneManager *sm, veViewer *viewer) override;

protected:

    VE_Ptr<veMesh>          _mesh;
	veReal                  _size;
	VE_Ptr<veRenderer>      _renderer;
	VE_Ptr<veMaterial>      _material;
    VE_Ptr<veRealArray>     _vertices;
    VE_Ptr<veUint16Array>   _indices;
};

#endif
