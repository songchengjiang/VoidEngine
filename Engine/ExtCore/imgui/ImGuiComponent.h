#ifndef _VE_IMGUI_COMPONENT_
#define _VE_IMGUI_COMPONENT_
#include "KernelCore/Component.h"
#include "KernelCore/Material.h"
#include "KernelCore/GLDataBuffer.h"
#include "imgui.h"

class VE_EXPORT veImGuiComponent : public veComponent
{
public:
    typedef std::function<void(veViewer *)> ImGuiRenderFunc;
    
    veImGuiComponent();
    virtual ~veImGuiComponent();
    
    virtual bool handle(veSceneManager *sm, veViewer *viewer, const veEvent &event) override;
    virtual void beforeUpdate(veSceneManager *sm) override;
    virtual void afterUpdate(veSceneManager *sm) override;
    virtual void beforeRender(veSceneManager *sm, veViewer *viewer) override;
    virtual void afterRender(veSceneManager *sm, veViewer *viewer) override;
    
    void setGuiRenderFunc(const ImGuiRenderFunc &func) { _renderFunc = func; }
    
protected:
    
    void initPass(veSceneManager *sm, unsigned int contextID);
    
protected:
    
    ImGuiRenderFunc _renderFunc;
    VE_Ptr<vePass>  _renderPass;
    VE_Ptr<veTexture>  _fontTexture;
    VE_Ptr<veUniform> _renderProjMatrix;
    
    VE_Ptr<veGLDataBuffer> _vaoBuffer;
    VE_Ptr<veGLDataBuffer> _vboBuffer;
    VE_Ptr<veGLDataBuffer> _iboBuffer;
    
    bool              _isAnyWindowFocus;
};

#endif
