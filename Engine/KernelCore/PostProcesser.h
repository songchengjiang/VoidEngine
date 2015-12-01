#ifndef _VE_POST_PROCESSER_
#define _VE_POST_PROCESSER_
#include "Prerequisites.h"
#include "Material.h"
#include "Surface.h"
#include "PostProcesserRenderer.h"

class veFrameBufferObject;
class VE_EXPORT vePostProcesser
{
	friend class veSceneManager;
public:
	~vePostProcesser();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	virtual void setMaterialArray(veMaterialArray *material) { _materials = material; _needRefresh = true; }
	veMaterialArray* getMaterialArray() { return _materials.get(); }

	void process(veFrameBufferObject *fb, veCamera *camera);
	void needRefresh() { _needRefresh = true; }

private:

	vePostProcesser(veSceneManager *sm);

private:

	VE_Ptr<veMaterialArray> _materials;
	VE_Ptr<veSurface> _surface;
	VE_Ptr<vePostProcesserRenderer> _renderer;
	veSceneManager   *_sceneManager;
	bool _needRefresh;
};

typedef std::vector< VE_Ptr<vePostProcesser> > vePostProcesserList;

#endif