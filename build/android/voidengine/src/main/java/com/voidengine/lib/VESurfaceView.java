package com.voidengine.lib;

import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.renderscript.Matrix4f;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceHolder;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;
import javax.microedition.khronos.opengles.GL10;

public class VESurfaceView extends GLSurfaceView {
    private static final String TAG = "VESurfaceView";
    private Renderer mRenderer;
    private boolean mNeedCreate = true;

    public VESurfaceView(Context context) {
        super(context);
        // Pick an EGLConfig with RGBA8 color, 24-bit depth, 8-bit stencil,
        // supporting OpenGL ES 2.0 or later backwards-compatible versions.

        //setEGLConfigChooser(8, 8, 8, 8, 24, 8);
        setEGLConfigChooser(new VESurfaceViewChooser());
        setEGLContextClientVersion(2);
        setFocusableInTouchMode(true);
        mRenderer = new Renderer(this);
        setRenderer(mRenderer);
    }


    public class VESurfaceViewChooser implements GLSurfaceView.EGLConfigChooser {

        @Override
        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display)
        {
            int[] EGLattribs = {
                    EGL10.EGL_RED_SIZE, 8,
                    EGL10.EGL_GREEN_SIZE, 8,
                    EGL10.EGL_BLUE_SIZE, 8,
                    EGL10.EGL_ALPHA_SIZE, 8,
                    EGL10.EGL_DEPTH_SIZE, 24,
                    EGL10.EGL_STENCIL_SIZE,8,
                    //EGL10.EGL_RENDERABLE_TYPE, 0x0040, //EGL_OPENGL_ES3_BIT_KHR
                    EGL10.EGL_SURFACE_TYPE, EGL10.EGL_WINDOW_BIT | EGL10.EGL_PBUFFER_BIT,
                    EGL10.EGL_NONE
            };
            EGLConfig[] configs = new EGLConfig[1];
            int[] numConfigs = new int[1];
            boolean eglChooseResult = egl.eglChooseConfig(display, EGLattribs, configs, 1, numConfigs);
            if (eglChooseResult && numConfigs[0] > 0) {
                return configs[0];
            }
            return null;
        }


    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        this.queueEvent(new VESurfaceRunnable(this) {
            @Override
            public void run() {
                if (!mSurfaceView.mNeedCreate) {
                    VEJNIWrapper.nativeOnSurfaceDestroy(mSurfaceView);
                }
            }
        });
        super.surfaceDestroyed(holder);
    }

    abstract public class VESurfaceRunnable implements Runnable {
        protected VESurfaceView mSurfaceView;
        VESurfaceRunnable(VESurfaceView sv){
            mSurfaceView = sv;
        }
    }

    @Override
    public void onPause() {
        mNeedCreate = false;
        this.queueEvent(new VESurfaceRunnable(this) {
            @Override
            public void run() {
                VEJNIWrapper.nativeOnSurfacePause(mSurfaceView);
            }
        });
        this.setRenderMode(RENDERMODE_WHEN_DIRTY);
        //super.onPause();
    }

    @Override
    public void onResume() {
        mNeedCreate = true;
        super.onResume();
        this.setRenderMode(RENDERMODE_CONTINUOUSLY);
        this.queueEvent(new VESurfaceRunnable(this) {
            @Override
            public void run() {
                VEJNIWrapper.nativeOnSurfaceResume(mSurfaceView);
            }
        });
    }

    @Override
    protected void onSizeChanged(final int pNewSurfaceWidth, final int pNewSurfaceHeight
                               , final int pOldSurfaceWidth, final int pOldSurfaceHeight) {
        mRenderer.setSurfaceSize(pNewSurfaceWidth, pNewSurfaceHeight);
    }

    @Override
    public boolean onTouchEvent(final MotionEvent pMotionEvent) {
        // these data are used in ACTION_MOVE and ACTION_CANCEL
        final int pointerNumber = pMotionEvent.getPointerCount();
        final int[] ids = new int[pointerNumber];
        final float[] xs = new float[pointerNumber];
        final float[] ys = new float[pointerNumber];

        for (int i = 0; i < pointerNumber; i++) {
            ids[i] = pMotionEvent.getPointerId(i);
            xs[i] = pMotionEvent.getX(i);
            ys[i] = pMotionEvent.getY(i);
        }

        switch (pMotionEvent.getAction() & MotionEvent.ACTION_MASK) {
            case MotionEvent.ACTION_POINTER_DOWN:
                final int indexPointerDown = pMotionEvent.getAction() >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
                final int idPointerDown = pMotionEvent.getPointerId(indexPointerDown);
                final float xPointerDown = pMotionEvent.getX(indexPointerDown);
                final float yPointerDown = pMotionEvent.getY(indexPointerDown);

                VEJNIWrapper.nativeOnSurfaceTouchDown(this, idPointerDown, xPointerDown, yPointerDown);
                break;

            case MotionEvent.ACTION_DOWN:
                // there are only one finger on the screen
                final int idDown = pMotionEvent.getPointerId(0);
                final float xDown = xs[0];
                final float yDown = ys[0];

                VEJNIWrapper.nativeOnSurfaceTouchDown(this, idDown, xDown, yDown);
                //VEJNIWrapper.nativeOnSetNodeVisible(mLightNode, !mIsVisible);
                //VEJNIWrapper.nativeOnSetNodeVisible(mModelID, !mIsVisible);
                mIsVisible = !mIsVisible;
                break;

            case MotionEvent.ACTION_MOVE:
                VEJNIWrapper.nativeOnSurfaceTouchMove(this, ids, xs, ys);
                break;

            case MotionEvent.ACTION_POINTER_UP:
                final int indexPointUp = pMotionEvent.getAction() >> MotionEvent.ACTION_POINTER_INDEX_SHIFT;
                final int idPointerUp = pMotionEvent.getPointerId(indexPointUp);
                final float xPointerUp = pMotionEvent.getX(indexPointUp);
                final float yPointerUp = pMotionEvent.getY(indexPointUp);

                VEJNIWrapper.nativeOnSurfaceTouchUp(this, idPointerUp, xPointerUp, yPointerUp);
                break;

            case MotionEvent.ACTION_UP:
                // there are only one finger on the screen
                final int idUp = pMotionEvent.getPointerId(0);
                final float xUp = xs[0];
                final float yUp = ys[0];

                VEJNIWrapper.nativeOnSurfaceTouchUp(this, idUp, xUp, yUp);
                break;

            case MotionEvent.ACTION_CANCEL:
                VEJNIWrapper.nativeOnSurfaceTouchCancel(this, ids, xs, ys);
                break;
        }

        return true;
    }

    public void onDestroy() {
        VEJNIWrapper.nativeOnSurfaceDestroy(this);
    }

    private static class Renderer implements GLSurfaceView.Renderer {
        private VESurfaceView mSurfaceView;
        private int mSurfaceWidth;
        private int mSurfaceHeight;

        public Renderer(VESurfaceView surfaceView) {
            mSurfaceView = surfaceView;
        }

        public void setSurfaceSize(int width, int height) {
            mSurfaceWidth = width;
            mSurfaceHeight = height;
        }

        public void onDrawFrame(GL10 gl) {
            GLES20.glClearColor(0.5f,0.5f,0.5f,1.0f);
            GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
            GLES20.glViewport(0,0,mSurfaceWidth,mSurfaceHeight);
            VEJNIWrapper.nativeOnSurfaceDrawFrame(mSurfaceView);
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            VEJNIWrapper.nativeOnSurfaceChanged(mSurfaceView, width, height);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            if (mSurfaceView.mNeedCreate) {
                VEJNIWrapper.nativeOnSurfaceCreated(mSurfaceView, mSurfaceWidth, mSurfaceHeight);

                long sm = VEJNIWrapper.nativeOnCreateSceneManager();
                long sceneNode = VEJNIWrapper.nativeOnGetSceneNode(sm);
                long mCameraID = VEJNIWrapper.nativeOnCreateCameraNode(sm, sceneNode, "MainCamera", 52.2f, (float) mSurfaceWidth / (float) mSurfaceHeight, 0.1f, 1000.0f);
                Matrix4f mat = new Matrix4f();
                mat.loadIdentity();
                mat.translate(0.0f, 0.0f, -1.0f);
                mat.rotate(90.0f, 1.0f, 0.0f, 0.0f);
                mat.scale(0.1f, 0.1f, 0.1f);

                //VEJNIWrapper.nativeOnMoveNode(mCameraID, offsetMat.getArray());
                mSurfaceView.mModelID = VEJNIWrapper.nativeOnCreateEmptyNode(sm, sceneNode, "Entity");
                VEJNIWrapper.nativeOnMoveNode(mSurfaceView.mModelID, mat.getArray());
                VEJNIWrapper.nativeOnCreateModelNode(sm, mSurfaceView.mModelID, "lamp", "models/lamp/lamp.vem");

                mat.loadTranslate(0.0f, 3.5f, 0.0f);
                mSurfaceView.mLightNode = VEJNIWrapper.nativeOnCreateLightNode(sm, mSurfaceView.mModelID, "pointLight", "lights/point0.velight");
                VEJNIWrapper.nativeOnMoveNode(mSurfaceView.mLightNode, mat.getArray());

                mat.loadScale(0.07f, 0.07f, 0.07f);
                VEJNIWrapper.nativeOnMoveNode(VEJNIWrapper.nativeOnCreateModelNode(sm, mSurfaceView.mModelID, "plane", "models/plane.vem"), mat.getArray());
                VEJNIWrapper.nativeOnMoveNode(VEJNIWrapper.nativeOnCreateModelNode(sm, mSurfaceView.mModelID, "shadowPlane", "models/shadowPlane.vem"), mat.getArray());

                VEJNIWrapper.nativeOnCreateComponent(mSurfaceView.mModelID, "NodeManipulator");

                Matrix4f res = new Matrix4f(VEJNIWrapper.nativeOnGetNodeMatrix(mSurfaceView.mModelID));
                VEJNIWrapper.nativeOnMoveNode(mSurfaceView.mModelID, res.getArray());



                //VEJNIWrapper.nativeOnRemoveCamera(camera);

                //VEJNIWrapper.nativeOnRemoveCamera(camera);
            }
            Log.i(TAG, "nativeOnSurfaceCreated");
        }

    }

    private long mLightNode = -1;
    private long mModelID = -1;
    private boolean mIsVisible = true;
}
