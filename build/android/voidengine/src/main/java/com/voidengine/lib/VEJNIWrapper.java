package com.voidengine.lib;

import android.app.Activity;
import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.util.Log;

public class VEJNIWrapper {

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
}
