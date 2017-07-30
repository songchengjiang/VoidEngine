#include <jni.h>
#include <android/asset_manager_jni.h>
#include <errno.h>
#include "VoidEngine.h"
#include "FileCore/File-android.h"
#include "KernelCore/Application-android.h"
#include "KernelCore/android/Viewer-android.h"
//#include "TestCreator.h"

class MaterialFinder : public veNodeVisitor
{
public:
    MaterialFinder(const std::string &name)
        : _name(name)
    {}

    virtual bool visit(veNode &node) override
    {
        if (0 < node.getRenderableObjectCount()) {
            for (unsigned int i = 0; i < node.getRenderableObjectCount(); ++i) {
                auto material = node.getRenderableObject(i)->getMaterial();
                if (material->getName() == _name) {
                    Material = material;
                    return true;
                }
            }
        }
        return false;
    }

    veMaterial *Material;

private:

    std::string _name;
};

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
    veApplication::instance()->createViewer(width, height, "VoidEngineTest");
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
    veApplication::instance()->destroyAllViewer();
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
    //veLog("nativeOnDrawFrame");
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

/*
 * Class:     com_voidengine_lib_VEJNIWrapper
 * Method:    nativeOnActivityCreate
 * Signature: (Landroid/app/Activity;Landroid/content/res/AssetManager;)V
 */
JNIEXPORT void JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnActivityCreate
        (JNIEnv *env, jclass jthis, jobject activity, jobject assetMgr) {
    veFileAndroid::ASSET_MANAGER = AAssetManager_fromJava(env, assetMgr);
}

/*
 * Class:     com_voidengine_lib_VEJNIWrapper
 * Method:    nativeOnActivityDestroy
 * Signature: (Landroid/app/Activity;)V
 */
JNIEXPORT void JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnActivityDestroy
        (JNIEnv *env, jclass jthis, jobject activity) {
    //veApplication::instance()->stop();
    //veApplication::instance()->destroyAllViewer();
    //veLog("nativeOnActivityDestroy");
}

JNIEXPORT long JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnCreateSceneManager
        (JNIEnv *env, jclass jthis) {

    auto mainViewer = veApplication::instance()->getViewer(0);
    veSceneManager *sceneManager = mainViewer->getSceneManager();
    if (!sceneManager) {
        sceneManager = new veOctreeSceneManager(veBoundingBox(veVec3(-1000.0f), veVec3(1000.0f)), 8);
        mainViewer->setSceneManager(sceneManager);
    }
    return reinterpret_cast<jlong>(sceneManager);
}

JNIEXPORT jlong JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnGetSceneNode
        (JNIEnv *env, jclass jthis, jlong sceneManagerID, jstring name) {

    veSceneManager *sceneManager = reinterpret_cast<veSceneManager *>(sceneManagerID);
    return reinterpret_cast<jlong>(sceneManager->getRootNode());
}

JNIEXPORT jlong JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnCreateEmptyNode
        (JNIEnv *env, jclass jthis, jlong sceneManagerID, jlong parentNodeID, jstring name) {

    veSceneManager *sceneManager = reinterpret_cast<veSceneManager *>(sceneManagerID);
    veNode *parent = reinterpret_cast<veNode *>(parentNodeID);

    const char *nativeString = env->GetStringUTFChars(name, 0);
    veNode *node = sceneManager->createNode(nativeString);
    //node->addComponent(new veNodeManipulator());
    env->ReleaseStringUTFChars(name, nativeString);
    parent->addChild(node);
    return reinterpret_cast<jlong>(node);
}

JNIEXPORT jlong JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnCreateModelNode
        (JNIEnv *env, jclass jthis, jlong sceneManagerID, jlong parentNodeID, jstring name, jstring filePath) {

    veSceneManager *sceneManager = reinterpret_cast<veSceneManager *>(sceneManagerID);
    veNode *parent = reinterpret_cast<veNode *>(parentNodeID);

    const char *nameString = env->GetStringUTFChars(name, 0);
    const char *fileString = env->GetStringUTFChars(filePath, 0);
    veNode *entity = static_cast<veNode *>(veFile::instance()->readFile(sceneManager, fileString, nameString));

    env->ReleaseStringUTFChars(name, nameString);
    env->ReleaseStringUTFChars(filePath, fileString);
    parent->addChild(entity);
    return reinterpret_cast<jlong>(entity);
}

JNIEXPORT jlong JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnCreateLightNode
        (JNIEnv *env, jclass jthis, jlong sceneManagerID, jlong parentNodeID, jstring name, jstring filePath) {

    veSceneManager *sceneManager = reinterpret_cast<veSceneManager *>(sceneManagerID);
    veNode *parent = reinterpret_cast<veNode *>(parentNodeID);

    const char *nameString = env->GetStringUTFChars(name, 0);
    const char *fileString = env->GetStringUTFChars(filePath, 0);

    veNode *lightNode = sceneManager->createNode(nameString);
    veLight *light = static_cast<veLight *>(veFile::instance()->readFile(sceneManager, fileString, nameString));
    lightNode->addComponent(light);
    parent->addChild(lightNode);

    env->ReleaseStringUTFChars(name, nameString);
    env->ReleaseStringUTFChars(filePath, fileString);
    return reinterpret_cast<jlong>(lightNode);
}

JNIEXPORT jlong JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnCreateComponent
        (JNIEnv *env, jclass jthis, jlong attachedNodeID, jstring type) {

    veNode *attachedNode = reinterpret_cast<veNode *>(attachedNodeID);

    veComponent *component = nullptr;
    const char *typeString = env->GetStringUTFChars(type, 0);
    if (strcmp(typeString, "NodeManipulator") == 0) {
        component = new veNodeManipulator();
    }
    attachedNode->addComponent(component);

    env->ReleaseStringUTFChars(type, typeString);
    return reinterpret_cast<jlong>(component);
}

JNIEXPORT jlong JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnCreateCameraNode
        (JNIEnv *env, jclass jthis, jlong sceneManagerID, jlong parentNodeID, jstring name, jfloat fovy, jfloat aspectRadio, jfloat zNear, jfloat zFar) {
    auto mainViewer = veApplication::instance()->getViewer(0);
    veSceneManager *sceneManager = reinterpret_cast<veSceneManager *>(sceneManagerID);
    veNode *parent = reinterpret_cast<veNode *>(parentNodeID);

    const char *nameString = env->GetStringUTFChars(name, 0);
    veNode *cameraNode = sceneManager->createNode(nameString);
    auto camera = sceneManager->createCamera(nameString, {0, 0, mainViewer->width(), mainViewer->height() });
    env->ReleaseStringUTFChars(name, nameString);

    camera->setProjectionMatrixAsPerspective(fovy, aspectRadio, zNear, zFar);
    veTexture *irradiance = static_cast<veTexture *>(veFile::instance()->readFile(sceneManager, "textures/Alexs_irradiance.png", "irradiance"));
    irradiance->setWrapMode(veTexture::REPEAT);
    irradiance->setFilterMode(veTexture::LINEAR);
    veTexture *radiance = static_cast<veTexture *>(veFile::instance()->readFile(sceneManager, "textures/Alexs_radiance.png", "radiance"));
    radiance->setWrapMode(veTexture::REPEAT);
    radiance->setFilterMode(veTexture::LINEAR_MIP_MAP_LINEAR);
    veTexture *lut = static_cast<veTexture *>(veFile::instance()->readFile(sceneManager, "textures/ibl_brdf_lut.png", "lut"));
    lut->setWrapMode(veTexture::CLAMP);
    lut->setFilterMode(veTexture::LINEAR);
    camera->setIrradianceTexture(irradiance);
    camera->setRadianceTexture(radiance);
    camera->setLutTexture(lut);
    //camera->addComponent(new veCameraManipulator());
    cameraNode->addComponent(camera);
    parent->addChild(cameraNode);

    return reinterpret_cast<jlong>(cameraNode);
}

JNIEXPORT void JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnMoveNode
        (JNIEnv *env, jclass jthis, jlong nodeID, jfloatArray matrix) {

    veNode *node = reinterpret_cast<veNode *>(nodeID);
    jfloat *mat = env->GetFloatArrayElements(matrix, nullptr);
    node->setMatrix(veMat4(mat[0], mat[4],  mat[8], mat[12]
                         , mat[1], mat[5],  mat[9], mat[13]
                         , mat[2], mat[6], mat[10], mat[14]
                         , mat[3], mat[7], mat[11], mat[15]));
    env->ReleaseFloatArrayElements(matrix, mat, 0);
}

JNIEXPORT jfloatArray JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnGetNodeMatrix
        (JNIEnv *env, jclass jthis, jlong nodeID) {
    veNode *node = reinterpret_cast<veNode *>(nodeID);
    jfloatArray result;
    result = env->NewFloatArray(16);
    veMat4 mat = node->getMatrix();
    mat.transpose();
    env->SetFloatArrayRegion(result, 0, 16, mat[0]);
    return result;
}

JNIEXPORT long JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnGetModelMaterial
        (JNIEnv *env, jclass jthis, jlong nodeID, jstring materialName) {

    veNode *node = reinterpret_cast<veNode *>(nodeID);
    const char *nameString = env->GetStringUTFChars(materialName, 0);
    MaterialFinder finder(nameString);
    node->accept(finder);
    env->ReleaseStringUTFChars(materialName, nameString);
    return reinterpret_cast<jlong>(finder.Material);
}

JNIEXPORT void JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnSetMaterialParameter
        (JNIEnv *env, jclass jthis, jlong materialID, jstring paramName, jfloatArray values) {

    veMaterial *material = reinterpret_cast<veMaterial *>(materialID);
    const char *nameString = env->GetStringUTFChars(paramName, 0);
    auto uniform = material->getTechnique(0)->getPass(0)->getUniform(nameString);
    if (uniform) {
        jfloat *val = env->GetFloatArrayElements(values, nullptr);
        if (uniform->getType() == veUniform::Type::REAL) {
            uniform->setValue(val[0]);
        }else if ()
        env->ReleaseFloatArrayElements(values, val, 0);
    }
    env->ReleaseStringUTFChars(paramName, nameString);
}

JNIEXPORT void JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnSetNodeVisible
        (JNIEnv *env, jclass jthis, jlong nodeID, jboolean isVisible) {

    veNode *node = reinterpret_cast<veNode *>(nodeID);
    node->setVisible(isVisible);
}

JNIEXPORT void JNICALL Java_com_voidengine_lib_VEJNIWrapper_nativeOnDestroyNode
        (JNIEnv *env, jclass jthis, jlong nodeID) {
    veNode *node = reinterpret_cast<veNode *>(nodeID);
    node->getParent()->removeChild(node);
}

}