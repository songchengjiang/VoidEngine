package com.voidengine;

import android.app.Activity;
import android.os.Bundle;

public class VoidEngineActivity extends Activity {
    VoidEngineGLSurfaceView glSurfaceView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        System.loadLibrary("VoidEngine");
        glSurfaceView = new VoidEngineGLSurfaceView(getApplication());
        setContentView(glSurfaceView);
    }

    @Override protected void onPause() {
        super.onPause();
        glSurfaceView.onPause();
    }

    @Override protected void onResume() {
        super.onResume();
        glSurfaceView.onResume();
    }
}
