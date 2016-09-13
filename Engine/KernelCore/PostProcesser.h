#ifndef _VE_POST_PROCESSER_
#define _VE_POST_PROCESSER_
#include "Prerequisites.h"
#include "Material.h"
#include "Surface.h"
#include "PostProcesserRenderer.h"

class veFrameBufferObject;
class veRenderPipeline;
class VE_EXPORT vePostProcesser
{
	friend class veSceneManager;
public:
	~vePostProcesser();

	USE_VE_PTR;
	USE_NAME_PROPERTY;
    
    void setEnabled(bool isEnabled) { _isEnabled = isEnabled; }
    bool isEnabled() const { return _isEnabled; }
    
	void setMaterialArray(veMaterialArray *material) { _materials = material;}
	veMaterialArray* getMaterialArray() { return _materials.get(); }

	void process(veRenderPipeline *pipeline, veCamera *camera, veTexture* sceneColorTex, veTexture* sceneDepthTex, unsigned int contextID);
    veSurface* getProcesserSurface() { return _postProcesserSurface.get(); }

private:

	vePostProcesser(veSceneManager *sm);

private:
    bool                            _isEnabled;
	VE_Ptr<veMaterialArray>         _materials;
	VE_Ptr<vePostProcesserRenderer> _renderer;
    VE_Ptr<veFrameBufferObject>     _fbo;
    VE_Ptr<veSurface>               _postProcesserSurface;
    VE_Ptr<veTexture>               _processedTexture;
	veSceneManager                 *_sceneManager;
};

typedef std::vector< VE_Ptr<vePostProcesser> > vePostProcesserList;

#endif