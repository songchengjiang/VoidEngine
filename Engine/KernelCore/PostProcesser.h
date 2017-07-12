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
    
    void setEnabled(bool isEnabled);
    bool isEnabled() const { return _isEnabled; }
    
    void setMaterialArray(veMaterialArray *material) { _materials = material;}
    veMaterialArray* getMaterialArray() { return _materials.get(); }
    
    void process(veRenderPipeline *pipeline, veCamera *camera, bool firstProcesser, unsigned int contextID);
    
private:
    
    vePostProcesser(veSceneManager *sm);
    
private:
    bool                            _isEnabled;
    VE_Ptr<veMaterialArray>         _materials;
    VE_Ptr<vePostProcesserRenderer> _renderer;
    VE_Ptr<veFrameBufferObject>     _fbo;
    veSceneManager                 *_sceneManager;
};

typedef std::vector< VE_Ptr<vePostProcesser> > vePostProcesserList;

#endif
