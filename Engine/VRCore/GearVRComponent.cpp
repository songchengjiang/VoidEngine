#include <gearvr/VrApi/Include/VrApi_Types.h>
#include "GearVRComponent.h"
#if VE_PLATFORM == VE_PLATFORM_ANDROID
#include "KernelCore/Camera.h"
#include "KernelCore/SceneManager.h"
#include "KernelCore/Viewer.h"
#include <sstream>

veGearVRComponent::veGearVRComponent(JavaVM *vm, jobject activity)
    : _ovr(nullptr)
    , _frameIndex(0)
    , _onResume(false)
    , _needRefresh(true)
{
    _java.Vm = vm;
    _java.Vm->AttachCurrentThread(&_java.Env, nullptr);
    _java.ActivityObject = _java.Env->NewGlobalRef(activity);
    const ovrInitParms initParms = vrapi_DefaultInitParms(&_java);
    int32_t initResult = vrapi_Initialize(&initParms);
    if (initResult != VRAPI_INITIALIZE_SUCCESS) {
        veLog("Failed to initialize VrApi!");
    }

}

veGearVRComponent::~veGearVRComponent()
{
    vrapi_Shutdown();
}

void veGearVRComponent::onCreated(veViewer *viewer, int width, int height)
{
    createFrameBuffers(viewer);
    correctEyesCamera();
    ovrModeParms modeParms = vrapi_DefaultModeParms(&_java);
    _ovr = vrapi_EnterVrMode(&modeParms);
}

void veGearVRComponent::onDestroy(veViewer *viewer)
{
    vrapi_LeaveVrMode(_ovr);
    _ovr = nullptr;
    destroyFrameBuffers(viewer);
}

void veGearVRComponent::onChanged(veViewer *viewer, int width, int height)
{
}

void veGearVRComponent::onPause(veViewer *viewer)
{
}

void veGearVRComponent::onResume(veViewer *viewer)
{
}

void veGearVRComponent::beforeUpdate(veSceneManager *sm)
{

}

void veGearVRComponent::afterUpdate(veSceneManager *sm)
{
}

void veGearVRComponent::beforeRender(veSceneManager *sm, veViewer *viewer)
{
    if (!_ovr)
        return;
    _currentContexID = viewer->getContextID();
    ++_frameIndex;
    const double predictedDisplayTime = vrapi_GetPredictedDisplayTime(_ovr, _frameIndex);
    const ovrTracking baseTracking = vrapi_GetPredictedTracking(_ovr, predictedDisplayTime);
    const ovrHeadModelParms headModelParms = vrapi_DefaultHeadModelParms();
    _headTracking = vrapi_ApplyHeadModel(&headModelParms, &baseTracking);
    const ovrMatrix4f centerEyeViewMatrix = vrapi_GetCenterEyeViewMatrix( &headModelParms, &_headTracking, NULL );

    for (int eye = 0; eye < VRAPI_FRAME_LAYER_EYE_MAX; ++eye) {
        veCamera *eyeCam = static_cast<veCamera *>(_attachedNodeList[eye]);
        eyeCam->setFrameBufferObject(_frameBuffers[eye].fbos[_frameBuffers[eye].textureSwapChainIndex].get());

        const ovrMatrix4f eyeViewMatrix = vrapi_GetEyeViewMatrix(&headModelParms, &centerEyeViewMatrix, eye);
        _eyesCameraOriginViewMat[eye] = eyeCam->viewMatrix();
        eyeCam->setViewMatrix(veMat4(eyeViewMatrix.M[0][0], eyeViewMatrix.M[0][1], eyeViewMatrix.M[0][2], eyeViewMatrix.M[0][3]
                                   , eyeViewMatrix.M[1][0], eyeViewMatrix.M[1][1], eyeViewMatrix.M[1][2], eyeViewMatrix.M[1][3]
                                   , eyeViewMatrix.M[2][0], eyeViewMatrix.M[2][1], eyeViewMatrix.M[2][2], eyeViewMatrix.M[2][3]
                                   , eyeViewMatrix.M[3][0], eyeViewMatrix.M[3][1], eyeViewMatrix.M[3][2], eyeViewMatrix.M[3][3]) * _eyesCameraOriginViewMat[eye]);
    }
//    auto rot = _headTracking.HeadPose.Pose.Orientation;
//    veLog("HeadTracking: (%f, %f, %f, %f)", rot.x, rot.y, rot.z, rot.w);
}

void veGearVRComponent::afterRender(veSceneManager *sm, veViewer *viewer)
{
    if (!_ovr)
        return;
    auto frameParams = vrapi_DefaultFrameParms(&_java, VRAPI_FRAME_INIT_DEFAULT, vrapi_GetTimeInSeconds(), nullptr);
    frameParams.FrameIndex = _frameIndex;
    frameParams.MinimumVsyncs = 2;
    frameParams.PerformanceParms = vrapi_DefaultPerformanceParms();
    frameParams.Layers[VRAPI_FRAME_LAYER_TYPE_WORLD].Flags |= VRAPI_FRAME_LAYER_FLAG_CHROMATIC_ABERRATION_CORRECTION;
    for (int eye = 0; eye < VRAPI_FRAME_LAYER_EYE_MAX; ++eye) {
        veCamera *eyeCam = static_cast<veCamera *>(_attachedNodeList[eye]);
        eyeCam->setViewMatrix(_eyesCameraOriginViewMat[eye]);

        frameParams.Layers[VRAPI_FRAME_LAYER_TYPE_WORLD].Textures[eye].ColorTextureSwapChain = _frameBuffers[eye].colorTextureSwapChain;
        frameParams.Layers[VRAPI_FRAME_LAYER_TYPE_WORLD].Textures[eye].TextureSwapChainIndex = _frameBuffers[eye].textureSwapChainIndex;
        frameParams.Layers[VRAPI_FRAME_LAYER_TYPE_WORLD].Textures[eye].TexCoordsFromTanAngles = _texCoordsTanAnglesMatrix;
        frameParams.Layers[VRAPI_FRAME_LAYER_TYPE_WORLD].Textures[eye].HeadPose = _headTracking.HeadPose;
        _frameBuffers[eye].textureSwapChainIndex = (_frameBuffers[eye].textureSwapChainIndex + 1) % _frameBuffers[eye].textureSwapChainLength;
    }
    vrapi_SubmitFrame(_ovr, &frameParams);
}

void veGearVRComponent::beforeDraw(veSceneManager *sm, veCamera *camera)
{
    if (camera != _attachedNodeList[0] && camera != _attachedNodeList[1] && !_ovr)
        return;
    auto vp = camera->getViewport();
    sm->getRenderState(_currentContexID)->setScissorTest(true);
    sm->getRenderState(_currentContexID)->applyState();
    glScissor(0, 0, vp.width, vp.height);
}

void veGearVRComponent::afterDraw(veSceneManager *sm, veCamera *camera)
{
    if (camera != _attachedNodeList[0] && camera != _attachedNodeList[1] && !_ovr)
        return;
    auto vp = camera->getViewport();
    // Explicitly clear the border texels to black because OpenGL-ES does not support GL_CLAMP_TO_BORDER.
    {
        // Clear to fully opaque black.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // bottom
        glScissor(0, 0, vp.width, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        // top
        glScissor(0, vp.height - 1, vp.width, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        // left
        glScissor(0, 0, 1, vp.height);
        glClear(GL_COLOR_BUFFER_BIT);
        // right
        glScissor(vp.width - 1, 0, 1, vp.height);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    sm->getRenderState(_currentContexID)->setScissorTest(false);
    sm->getRenderState(_currentContexID)->applyState();
}

bool veGearVRComponent::onAttachToNode(veNode *node)
{
    if (2 <= _attachedNodeList.size()) return false;
    if (dynamic_cast<veCamera* >(node) == nullptr) return false;
    return veComponent::onAttachToNode(node);
}

void veGearVRComponent::createFrameBuffers(veViewer *viewer)
{
    int width  = vrapi_GetSystemPropertyInt(&_java, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_WIDTH);
    int height = vrapi_GetSystemPropertyInt(&_java, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_HEIGHT);

    for (int eye = 0; eye < VRAPI_FRAME_LAYER_EYE_MAX; ++eye) {
        _frameBuffers[eye].width = width;
        _frameBuffers[eye].height = height;
        _frameBuffers[eye].colorTextureSwapChain = vrapi_CreateTextureSwapChain(VRAPI_TEXTURE_TYPE_2D, VRAPI_TEXTURE_FORMAT_8888, width, height, 1, true);
        _frameBuffers[eye].textureSwapChainLength = vrapi_GetTextureSwapChainLength(_frameBuffers[eye].colorTextureSwapChain);
        _frameBuffers[eye].textureSwapChainIndex = 0;
        _frameBuffers[eye].fbos = new VE_Ptr<veFrameBufferObject>[_frameBuffers[eye].textureSwapChainLength];

        for (int i = 0; i < _frameBuffers[eye].textureSwapChainLength; ++i) {
            std::stringstream ss;
            ss <<eye<<"_"<<i;
            _frameBuffers[eye].fbos[i] = viewer->getSceneManager()->createFrameBufferObject("_GEARVR_COMPONENT_FBO_" + ss.str());
//            auto tex = viewer->getSceneManager()->createTexture("_GEARVR_COMPONENT_FBO_TEXTURE_" + ss.str());
//            tex->setFilterMode(veTexture::LINEAR);
//            tex->setWrapMode(veTexture::CLAMP);
//            tex->storage(width, height, 1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, nullptr, 1);
//            tex->bind(viewer->getContextID());
//            vrapi_SetTextureSwapChainHandle(_frameBuffers[eye].colorTextureSwapChain, i, tex->glTex(viewer->getContextID()));
//            _frameBuffers[eye].fbos[i]->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex);

            const GLuint colorTexture = vrapi_GetTextureSwapChainHandle(_frameBuffers[eye].colorTextureSwapChain, i);
            glBindTexture(GL_TEXTURE_2D, colorTexture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            _frameBuffers[eye].fbos[i]->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture);
        }
    }
}

void veGearVRComponent::destroyFrameBuffers(veViewer *viewer)
{
    for (int eye = 0; eye < VRAPI_FRAME_LAYER_EYE_MAX; ++eye) {
        VE_SAFE_DELETE_ARRAY(_frameBuffers[eye].fbos);
        vrapi_DestroyTextureSwapChain(_frameBuffers[eye].colorTextureSwapChain);
    }
}

void veGearVRComponent::correctEyesCamera()
{
    _projectionMatrix = ovrMatrix4f_CreateProjectionFov(
            vrapi_GetSystemPropertyFloat(&_java, VRAPI_SYS_PROP_SUGGESTED_EYE_FOV_DEGREES_X),
            vrapi_GetSystemPropertyFloat(&_java, VRAPI_SYS_PROP_SUGGESTED_EYE_FOV_DEGREES_Y),
            0.0f, 0.0f, 1.0f, 0.0f);
    _texCoordsTanAnglesMatrix = ovrMatrix4f_TanAngleMatrixFromProjection(&_projectionMatrix);

    for (int eye = 0; eye < VRAPI_FRAME_LAYER_EYE_MAX; ++eye){
        veCamera *eyeCam = static_cast<veCamera *>(_attachedNodeList[eye]);
        eyeCam->setViewport({0, 0, _frameBuffers[eye].width, _frameBuffers[eye].height});
        eyeCam->setProjectionMatrix(veMat4(_projectionMatrix.M[0][0], _projectionMatrix.M[0][1], _projectionMatrix.M[0][2], _projectionMatrix.M[0][3]
                                         , _projectionMatrix.M[1][0], _projectionMatrix.M[1][1], _projectionMatrix.M[1][2], _projectionMatrix.M[1][3]
                                         , _projectionMatrix.M[2][0], _projectionMatrix.M[2][1], _projectionMatrix.M[2][2], _projectionMatrix.M[2][3]
                                         , _projectionMatrix.M[3][0], _projectionMatrix.M[3][1], _projectionMatrix.M[3][2], _projectionMatrix.M[3][3]));
    }
}
#endif