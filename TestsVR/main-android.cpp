#include <jni.h>
#include <android/asset_manager_jni.h>
#include <errno.h>
#include <KernelCore/Application-android.h>
#include "VoidEngine.h"
#include "FileCore/File-android.h"
#include "TestCreator.h"

extern "C" {
/*
 * Class:     com_voidengine_lib_VEJNIWrapper
 * Method:    nativeOnActivityCreate
 * Signature: (Landroid/app/Activity;Landroid/content/res/AssetManager;)V
 */
JNIEXPORT void JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnActivityCreate
        (JNIEnv *env, jclass jthis, jobject activity, jobject assetMgr) {
    veFileAndroid::ASSET_MANAGER = AAssetManager_fromJava(env, assetMgr);

    veVRHelper vrHelper;
    vrHelper.javaVM = static_cast<veApplicationAndroid *>(veApplication::instance())->getJavaVM();
    vrHelper.javaVM->AttachCurrentThread(&vrHelper.javaEnv, nullptr);
    vrHelper.activity = vrHelper.javaEnv->NewGlobalRef(activity);
    veApplication::instance()->createViewer(800, 600, "VoidEngineTest");
    TestCreator::createTest(vrHelper);
    veApplication::instance()->run();

    veLog("nativeOnActivityCreate");
}

/*
 * Class:     com_voidengine_lib_VEJNIWrapper
 * Method:    nativeOnActivityDestroy
 * Signature: (Landroid/app/Activity;)V
 */
JNIEXPORT void JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnActivityDestroy
        (JNIEnv *env, jclass jthis, jobject activity) {
    veApplication::instance()->stop();
    veLog("nativeOnActivityDestroy");
}

}