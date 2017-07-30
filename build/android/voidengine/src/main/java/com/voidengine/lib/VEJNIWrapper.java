package com.voidengine.lib;

import android.app.Activity;
import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.util.Log;

public class VEJNIWrapper {

    static {
        System.loadLibrary("voidengine");
    }

    public static native void nativeOnActivityCreate(Activity activity, AssetManager assetMgr);
    public static native void nativeOnActivityDestroy(Activity activity);

    public static native void nativeOnSurfacePause(GLSurfaceView surface);
    public static native void nativeOnSurfaceResume(GLSurfaceView surface);
    public static native void nativeOnSurfaceCreated(GLSurfaceView surface, int width, int height);
    public static native void nativeOnSurfaceDestroy(GLSurfaceView surface);
    public static native void nativeOnSurfaceChanged(GLSurfaceView surface, int width, int height);
    public static native void nativeOnSurfaceDrawFrame(GLSurfaceView surface);
    public static native void nativeOnSurfaceTouchDown(GLSurfaceView surface, int id, float x, float y);
    public static native void nativeOnSurfaceTouchUp(GLSurfaceView surface, int id, float x, float y);
    public static native void nativeOnSurfaceTouchMove(GLSurfaceView surface, final int[] ids, final float[] xs, final float[] ys);
    public static native void nativeOnSurfaceTouchCancel(GLSurfaceView surface, final int[] ids, final float[] xs, final float[] ys);


    public static native long nativeOnCreateSceneManager();
    public static native long nativeOnGetSceneNode(long sceneManagerID);
    public static native long nativeOnCreateEmptyNode(long sceneManagerID, long parentNodeID, String name);
    public static native long nativeOnCreateModelNode(long sceneManagerID, long parentNodeID, String name, String filePath);
    public static native long nativeOnCreateCameraNode(long sceneManagerID, long parentNodeID, String name, float fovy, float aspectRadio, float zNear, float zFar);
    public static native long nativeOnCreateLightNode(long sceneManagerID, long parentNodeID, String name, String filePath);
    public static native long nativeOnCreateComponent(long attachedNodeID, String type);
    public static native void nativeOnMoveNode(long nodeID, final float[] matrix);
    public static native float[] nativeOnGetNodeMatrix(long nodeID);
    public static native long nativeOnGetModelMaterial(long nodeID, String materialName);
    public static native void nativeOnSetMaterialParameter(long materialID, String paramName, final float[] values);
    public static native void nativeOnSetNodeVisible(long nodeID, boolean isVisible);
    public static native void nativeOnDestroyNode(long modelID);
}
