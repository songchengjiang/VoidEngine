package com.voidengine.lib;

import android.app.Activity;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;

public class VEMainActivity extends Activity {

    private static final String TAG = "VEMainActivity";
    protected VESurfaceView mSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        onLoadLibrary();

        mSurfaceView = new VESurfaceView(this);
        setContentView(mSurfaceView);

        mSurfaceView.queueEvent(new VEActivityRunnable(this) {
            @Override
            public void run() {
                VEJNIWrapper.nativeOnActivityCreate(mActivity, mActivity.getAssets());
            }
        });
    }

    abstract public class VEActivityRunnable implements Runnable {
        protected VEMainActivity mActivity;
        VEActivityRunnable(VEMainActivity ac){
            mActivity = ac;
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        mSurfaceView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mSurfaceView.onResume();
    }

    @Override
    protected void onDestroy() {
        mSurfaceView.queueEvent(new VEActivityRunnable(this) {
            @Override
            public void run() {
                VEJNIWrapper.nativeOnActivityDestroy(mActivity);
            }
        });
        super.onDestroy();
    }

    protected void onLoadLibrary() {
        try {
            ApplicationInfo ai = getPackageManager().getApplicationInfo(getPackageName(), PackageManager.GET_META_DATA);
            Bundle bundle = ai.metaData;
            String libName = bundle.getString("android.app.lib_name");
            System.loadLibrary(libName);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
