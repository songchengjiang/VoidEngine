package com.voidengine.lib;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class VESurfaceView extends GLSurfaceView {
    private static final String TAG = "VESurfaceView";
    protected Renderer mRenderer;

    public VESurfaceView(Context context) {
        super(context);
        // Pick an EGLConfig with RGBA8 color, 24-bit depth, 8-bit stencil,
        // supporting OpenGL ES 2.0 or later backwards-compatible versions.
        setEGLConfigChooser(8, 8, 8, 8, 24, 8);
        setEGLContextClientVersion(2);
        setFocusableInTouchMode(true);
        mRenderer = new Renderer(this);
        setRenderer(mRenderer);
    }

    @Override
    public void onPause() {
        super.onPause();
        VEJNIWrapper.nativeOnSurfacePause(this);
    }

    @Override
    public void onResume() {
        super.onResume();
        VEJNIWrapper.nativeOnSurfaceResume(this);
    }

    @Override
    protected void onSizeChanged(final int pNewSurfaceWidth, final int pNewSurfaceHeight
                               , final int pOldSurfaceWidth, final int pOldSurfaceHeight) {
        mRenderer.setSurfaceSize(pNewSurfaceWidth, pNewSurfaceHeight);
    }

    public void onDestroy() {
        VEJNIWrapper.nativeOnSurfaceDestroy(this);
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
            VEJNIWrapper.nativeOnSurfaceDrawFrame(mSurfaceView);
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
            VEJNIWrapper.nativeOnSurfaceChanged(mSurfaceView, width, height);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            VEJNIWrapper.nativeOnSurfaceCreated(mSurfaceView, mSurfaceWidth, mSurfaceHeight);
        }
    }
}
