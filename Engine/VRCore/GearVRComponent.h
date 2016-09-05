#ifndef _GEARVR_COMPONENT_H_
#define _GEARVR_COMPONENT_H_

#include <BaseCore/Matrix4.h>
#include "KernelCore/Component.h"
#include "KernelCore/FrameBufferObject.h"

#if VE_PLATFORM == VE_PLATFORM_ANDROID
#include "KernelCore/android/ViewerListener-android.h"
#include "gearvr/VrApi/Include/VrApi.h"
#include "gearvr/VrApi/Include/VrApi_Helpers.h"

class VE_EXPORT veGearVRComponent : public veComponent, public veViewerListenerAndroid
{
public:
    veGearVRComponent(JavaVM *vm, jobject activity);
    virtual ~veGearVRComponent();

    virtual void beforeUpdate(veSceneManager *sm) override;
    virtual void afterUpdate(veSceneManager *sm) override;
    virtual void beforeRender(veSceneManager *sm, veViewer *viewer) override;
    virtual void afterRender(veSceneManager *sm, veViewer *viewer) override;
    virtual void beforeDraw(veSceneManager *sm, veCamera *camera) override;
    virtual void afterDraw(veSceneManager *sm, veCamera *camera) override;
    virtual bool onAttachToNode(veNode *node) override;

    virtual void onCreated(veViewer *viewer, int width, int height) override;
    virtual void onChanged(veViewer *viewer, int width, int height) override;
    virtual void onDestroy(veViewer *viewer) override;
    virtual void onPause(veViewer *viewer) override;
    virtual void onResume(veViewer *viewer) override;

protected:

    void createFrameBuffers(veViewer *viewer);
    void destroyFrameBuffers(veViewer *viewer);
    void correctEyesCamera();

protected:

    struct ovrFrameBuffer {
        VE_Ptr<veFrameBufferObject> *fbos;
        ovrTextureSwapChain *colorTextureSwapChain;
        int                  width;
        int                  height;
        int                  textureSwapChainLength;
        int                  textureSwapChainIndex;
    };

    ovrJava        _java;
    ovrMobile     *_ovr;
    ovrTracking    _headTracking;
    long long      _frameIndex;
    ovrFrameBuffer _frameBuffers[VRAPI_FRAME_LAYER_EYE_MAX];
    veMat4         _eyesCameraOriginViewMat[VRAPI_FRAME_LAYER_EYE_MAX];
    ovrMatrix4f	   _projectionMatrix;
    ovrMatrix4f    _texCoordsTanAnglesMatrix;

    unsigned int _currentContexID;
    bool         _onResume;
    bool         _isInited;
    bool         _needRefresh;
};
#endif

#endif
