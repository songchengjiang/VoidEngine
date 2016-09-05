#include "Application-android.h"
#include "FileCore/File-android.h"
#include "KernelCore/android/Viewer-android.h"
#include <android/input.h>
#include <android/asset_manager_jni.h>

//extern void android_activity_onCreated(JNIEnv *env, jobject activity) __attribute__((weak));
//extern void android_activity_onDestroy(JNIEnv *env, jobject activity) __attribute__((weak));

extern "C" {

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    static_cast<veApplicationAndroid *>(veApplication::instance())->setJavaVM(vm);
    veLog("VE: JNI_OnLoad");
    return JNI_VERSION_1_4;
}
/*
 * Class:     com_voidengine_lib_VEJNIWrapper
 * Method:    nativeOnSurfacePause
 * Signature: (Landroid/opengl/GLSurfaceView;)V
 */
JNIEXPORT void JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnSurfacePause
        (JNIEnv *env, jclass jthis, jobject surfaceview)
{
    if (0 < veApplication::instance()->getViewerCount()) {
        static_cast<veViewerAndroid *>(veApplication::instance()->getViewer(0))->onPause();
    }
    veLog("nativeOnSurfacePause");
}

/*
 * Class:     com_voidengine_lib_VEJNIWrapper
 * Method:    nativeOnSurfaceResume
 * Signature: (Landroid/opengl/GLSurfaceView;)V
 */
JNIEXPORT void JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnSurfaceResume
        (JNIEnv *env, jclass jthis, jobject surfaceview)
{
    if (0 < veApplication::instance()->getViewerCount()) {
        static_cast<veViewerAndroid *>(veApplication::instance()->getViewer(0))->onResume();
    }
    veLog("nativeOnSurfaceResume");
}

/*
 * Class:     com_voidengine_lib_VEJNIWrapper
 * Method:    nativeOnSurfaceCreated
 * Signature: (Landroid/opengl/GLSurfaceView;)V
 */
JNIEXPORT void JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnSurfaceCreated
        (JNIEnv *env, jclass jthis, jobject surfaceview, jint width, jint height)
{
    if (0 < veApplication::instance()->getViewerCount()) {
        static_cast<veViewerAndroid *>(veApplication::instance()->getViewer(0))->onCreated(width, height);
    }
    veLog("nativeOnSurfaceCreated");
}

/*
 * Class:     com_voidengine_lib_VEJNIWrapper
 * Method:    nativeOnSurfaceDestroy
 * Signature: (Landroid/opengl/GLSurfaceView;)V
 */
JNIEXPORT void JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnSurfaceDestroy
        (JNIEnv *env, jclass jthis, jobject surfaceview)
{
    if (0 < veApplication::instance()->getViewerCount()) {
        static_cast<veViewerAndroid *>(veApplication::instance()->getViewer(0))->onDestroy();
    }
    veLog("nativeOnSurfaceDestroy");
}

/*
 * Class:     com_voidengine_lib_VEJNIWrapper
 * Method:    nativeOnSurfaceChanged
 * Signature: (Landroid/opengl/GLSurfaceView;II)V
 */
JNIEXPORT void JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnSurfaceChanged
        (JNIEnv *env, jclass jthis, jobject surfaceview, jint width, jint height)
{
    if (0 < veApplication::instance()->getViewerCount()) {
        static_cast<veViewerAndroid *>(veApplication::instance()->getViewer(0))->onChanged(width, height);
    }
    veLog("nativeOnSurfaceChanged");
}

/*
 * Class:     com_voidengine_lib_VEJNIWrapper
 * Method:    nativeOnSurfaceDrawFrame
 * Signature: (Landroid/opengl/GLSurfaceView;)V
 */
JNIEXPORT void JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnSurfaceDrawFrame
        (JNIEnv *env, jclass jthis, jobject surfaceview)
{
    if (0 < veApplication::instance()->getViewerCount()) {
        static_cast<veViewerAndroid *>(veApplication::instance()->getViewer(0))->onDrawFrame();
    }
//    veLog("nativeOnDrawFrame");
}

/*
 * Class:     com_voidengine_lib_VEJNIWrapper
 * Method:    nativeOnSurfaceTouchDown
 * Signature: (Landroid/opengl/GLSurfaceView;IFF)V
 */
JNIEXPORT void JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnSurfaceTouchDown
        (JNIEnv *env, jclass jthis, jobject surfaceview, jint id, jfloat x, jfloat y)
{
    if (0 < veApplication::instance()->getViewerCount()) {
        static_cast<veViewerAndroid *>(veApplication::instance()->getViewer(0))->onTouchBegan(id, x, y);
    }
    //veLog("nativeOnSurfaceTouchDown(%d, %f, %f)", id, x, y);
}

/*
 * Class:     com_voidengine_lib_VEJNIWrapper
 * Method:    nativeOnSurfaceTouchUp
 * Signature: (Landroid/opengl/GLSurfaceView;IFF)V
 */
JNIEXPORT void JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnSurfaceTouchUp
        (JNIEnv *env, jclass jthis, jobject surfaceview, jint id, jfloat x, jfloat y)
{
    if (0 < veApplication::instance()->getViewerCount()) {
        static_cast<veViewerAndroid *>(veApplication::instance()->getViewer(0))->onTouchEnd(id, x, y);
    }
    //veLog("nativeOnSurfaceTouchUp(%d, %f, %f)", id, x, y);
}

/*
 * Class:     com_voidengine_lib_VEJNIWrapper
 * Method:    nativeOnSurfaceTouchMove
 * Signature: (Landroid/opengl/GLSurfaceView;[I[F[F)V
 */
JNIEXPORT void JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnSurfaceTouchMove
        (JNIEnv *env, jclass jthis, jobject surfaceview, jintArray ids, jfloatArray xs, jfloatArray ys)
{
    if (0 < veApplication::instance()->getViewerCount()) {
        static_cast<veViewerAndroid *>(veApplication::instance()->getViewer(0))->onTouchsMove(env->GetArrayLength(ids),
                                                                                              env->GetIntArrayElements(ids, nullptr),
                                                                                              env->GetFloatArrayElements(xs, nullptr),
                                                                                              env->GetFloatArrayElements(ys, nullptr));
    }
}

/*
 * Class:     com_voidengine_lib_VEJNIWrapper
 * Method:    nativeOnSurfaceTouchCancel
 * Signature: (Landroid/opengl/GLSurfaceView;[I[F[F)V
 */
JNIEXPORT void JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnSurfaceTouchCancel
        (JNIEnv *env, jclass jthis, jobject surfaceview, jintArray ids, jfloatArray xs, jfloatArray ys)
{

}

}

veApplicationAndroid::veApplicationAndroid()
//    : _androidApp(nullptr)
{

}

veApplicationAndroid::~veApplicationAndroid() {
    stop();
    for (auto &viewer : _viewerList) {
        VE_SAFE_DELETE(viewer);
    }
}

veViewer* veApplicationAndroid::createViewer(int width, int height, const std::string &title, veViewer *sharedContextViewer)
{
    auto viewer = new veViewerAndroid(width, height, title, static_cast<veViewerAndroid *>(sharedContextViewer));
    _viewerList.push_back(viewer);
    return viewer;
}

//void veApplicationAndroid::setAndroidApp(android_app *app)
//{
//    _androidApp = app;
//    _androidApp->userData = this;
//    _androidApp->onAppCmd = veApplicationAndroid::collectWindowEvents;
//    _androidApp->onInputEvent = veApplicationAndroid::collectInputEvents;
//    veFileAndroid::ASSET_MANAGER = _androidApp->activity->assetManager;
//}

bool veApplicationAndroid::run()
{
    if (_viewerList.empty()) return false;

    for (auto &viewer : _viewerList) {
        viewer->create();
        viewer->startSimulation();
    }
    return true;
}

void veApplicationAndroid::stop()
{
    for (auto &viewer : _viewerList) {
        viewer->stopSimulation();
        viewer->destroy();
    }
}

//void veApplicationAndroid::pollAllEvents() {
//    int ident = 0;
//    int events = 0;
//    struct android_poll_source* source = nullptr;
//    while ((ident=ALooper_pollAll(_isRunning ? 0 : -1, NULL, &events,
//                                  (void**)&source)) >= 0) {
//        // Process this event.
//        if (source != NULL) {
//            source->process(_androidApp, source);
//        }
//    }
//}

//void veApplicationAndroid::collectWindowEvents(struct android_app *app, int32_t cmd) {
//    veApplicationAndroid* thisApp = static_cast<veApplicationAndroid *>(app->userData);
//    veEvent &event = thisApp->_currentEvent;
//    switch (cmd) {
//        case APP_CMD_SAVE_STATE:
//            break;
//        case APP_CMD_INIT_WINDOW:
//        {
//            thisApp->init();
//            event.setEventType(veEvent::VE_WIN_RESIZE);
//            event.setWindowWidth(thisApp->_viewerList[0]->width());
//            event.setWindowHeight(thisApp->_viewerList[0]->height());
//        }
//            break;
//        case APP_CMD_TERM_WINDOW:
//        {
//            thisApp->terminate();
//            event.setEventType(veEvent::VE_WIN_CLOSE);
//        }
//            break;
//        case APP_CMD_GAINED_FOCUS:
//        {
//            event.setEventType(veEvent::VE_WIN_FOCUS);
//        }
//            break;
//        case APP_CMD_LOST_FOCUS:
//        {
//            event.setEventType(veEvent::VE_WIN_NOFOCUS);
//        }
//            break;
//    }
//}
//
//int32_t veApplicationAndroid::collectInputEvents(struct android_app *app, AInputEvent *aEvent) {
//    veApplicationAndroid* thisApp = static_cast<veApplicationAndroid *>(app->userData);
//    veEvent &event = thisApp->_currentEvent;
//    bool state = false;
//    if (AInputEvent_getType(aEvent) == AINPUT_EVENT_TYPE_MOTION){
//        int32_t action = AMotionEvent_getAction( aEvent );
//        int32_t index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
//                        >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
//        uint32_t flags = action & AMOTION_EVENT_ACTION_MASK;
//        int32_t count = AMotionEvent_getPointerCount( aEvent );
//
//        switch( flags )
//        {
//            case AMOTION_EVENT_ACTION_DOWN:
//            case AMOTION_EVENT_ACTION_UP:
//            case AMOTION_EVENT_ACTION_POINTER_DOWN:
//            case AMOTION_EVENT_ACTION_POINTER_UP:
//            {
//                int32_t idx = (flags == AMOTION_EVENT_ACTION_DOWN || flags == AMOTION_EVENT_ACTION_UP)? 0: index;
//                int32_t id = AMotionEvent_getPointerId(aEvent, idx);
//                if (flags == AMOTION_EVENT_ACTION_DOWN || flags == AMOTION_EVENT_ACTION_POINTER_DOWN){
//                    veReal x = (AMotionEvent_getX(aEvent, idx) / veReal(thisApp->_viewerList[0]->width()));
//                    veReal y = 1.0 - AMotionEvent_getY(aEvent, idx) / veReal(thisApp->_viewerList[0]->height());
//                    x = (x - 0.5) * 2.0;
//                    y = (y - 0.5) * 2.0;
//                    event.addTouch({id, x, y, x, y});
//                    event.setEventType(veEvent::VE_TOUCH_START);
//                   //veLog("VE_TOUCH_START(%d): (%f, %f)", id, x, y);
//                }else{
//                    event.removeTouch(id);
//                    event.setEventType(veEvent::VE_TOUCH_END);
//                    //veLog("VE_TOUCH_END");
//                }
//            }
//                break;
//            case AMOTION_EVENT_ACTION_MOVE:
//            {
//                for (int32_t i = 0; i < count; ++i){
//                    veReal x = (AMotionEvent_getX(aEvent, i) / veReal(thisApp->_viewerList[0]->width()));
//                    veReal y = 1.0 - AMotionEvent_getY(aEvent, i) / veReal(thisApp->_viewerList[0]->height());
//                    x = (x - 0.5) * 2.0;
//                    y = (y - 0.5) * 2.0;
//                    int32_t id = AMotionEvent_getPointerId(aEvent, i);
//                    auto &touches = event.getTouches();
//                    for (auto iter = touches.begin(); iter != touches.end(); ++iter) {
//                        if (iter->id == id) {
//                            iter->latestx = iter->x;
//                            iter->latesty = iter->y;
//                            iter->x = x;
//                            iter->y = y;
//                            //veLog("VE_TOUCH_MOVE(%d): (%f, %f, %f, %f)", id, iter->latestx, iter->latesty, iter->x, iter->y);
//                        }
//                    }
//                }
//                if (0 < count){
//                    event.setEventType(veEvent::VE_TOUCH_MOVE);
//                }
//            }
//                break;
//            case AMOTION_EVENT_ACTION_CANCEL:
//                break;
//        }
//        state = true;
//    }else if (AInputEvent_getType(aEvent) == AINPUT_EVENT_TYPE_KEY){
//        state = true;
//    }
//    if (state){
//        return 1;
//    }
//    return 0;
//}