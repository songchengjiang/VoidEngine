package com.voidengine;

import android.content.Context;
import android.opengl.GLSurfaceView;

/**
 * Created by Administrator on 2015/11/1.
 */
public class VoidEngineGLSurfaceView extends GLSurfaceView{
    public  VoidEngineRenderer renderer;
    public  VoidEngineGLSurfaceView(Context context){
        super(context);
        setEGLConfigChooser(8, 8, 8, 8, 24, 0);
        setEGLContextClientVersion(2);
        renderer = new VoidEngineRenderer();
        VoidEngineRenderer.setAssertManager(context.getAssets());
        setRenderer(renderer);
    }
}
