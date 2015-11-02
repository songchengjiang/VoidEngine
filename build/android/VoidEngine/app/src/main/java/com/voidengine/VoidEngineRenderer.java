package com.voidengine;

import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by Administrator on 2015/11/1.
 */
public class VoidEngineRenderer implements GLSurfaceView.Renderer{

    public void onDrawFrame(GL10 gl) {
        frame();
    }

    public void onSurfaceChanged(GL10 gl, int width, int height) {
        resize(width, height);
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        init();
    }

    public static native void init();
    public static native void resize(int width, int height);
    public static native void frame();
    public static native void setAssertManager(Object assertManager);
}
