package com.gluonhq.helloandroid;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.graphics.PixelFormat;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.view.inputmethod.InputMethodManager;
import android.widget.FrameLayout;

public class MainActivity extends Activity implements SurfaceHolder.Callback,
        SurfaceHolder.Callback2 {

    private static MainActivity   instance;
    private static FrameLayout  mViewGroup;
    private static SurfaceView  mView;
    private long nativeWindowPtr;
    private static final String TAG     = "GraalActivity";


   @Override
   protected void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);
System.err.println("Hello WORLD ACTIVITY, onCreate called");
            super.onCreate(savedInstanceState);

        getWindow().requestFeature(Window.FEATURE_NO_TITLE);
        getWindow().setSoftInputMode(
                WindowManager.LayoutParams.SOFT_INPUT_STATE_UNSPECIFIED
                | WindowManager.LayoutParams.SOFT_INPUT_ADJUST_NOTHING);
        getWindow().setFormat(PixelFormat.RGBA_8888);


        mView = new InternalSurfaceView(this);
        mView.getHolder().addCallback(this);
        mViewGroup = new FrameLayout(this);
        mViewGroup.addView(mView);
        setContentView(mViewGroup);
        instance = this;
   }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.v(TAG, "[MainGraalActivity] Surface created in activity.");
        Log.v(TAG, "loading Graallib");
        System.loadLibrary("mygraal");
        Log.v(TAG, "loaded Graallib");
        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metrics);
        float density = metrics.density;
        Log.v(TAG, "metrics = "+metrics+", density = "+density);
        nativeWindowPtr = surfaceReady(holder.getSurface(), density);
        Log.v(TAG, "Surface created, native code informed about it, nativeWindow at "+nativeWindowPtr);
        Log.v(TAG, "We will now launch Graal in a separate thread");
        Thread t = new Thread() {
            @Override public void run() {
                try {
                    startGraalApp();
                } catch (Throwable t) {
                    t.printStackTrace();
                }
            }
        };
        t.start();
    }

    private native void startGraalApp();
    // private native void testGL();
    private native long surfaceReady(Surface surface, float density);


    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width,
                    int height) {
    Log.v(TAG, "[MainActivity] surfaceChange, format = "+format+", width = "+width+", height = "+height);
System.err.println("[MainActivity] surfaceChange, format = "+format+", width = "+width+", height = "+height);
DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metrics);
        float density = metrics.density;
System.err.println("surfaceChanged, metrics = "+metrics+", density = "+density);
            // _surfaceChanged(holder.getSurface(), format, width, height);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
System.err.println("[MainGraalActivity] surfaceDestroyed");
            // _surfaceChanged(null);
    }

    @Override
    public void surfaceRedrawNeeded(SurfaceHolder holder) {
        System.err.println("[MainGraalActivity] surfaceRedrawNeeded. For now, we ignore this");
        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metrics);
        float density = metrics.density;
    }

    class InternalSurfaceView extends SurfaceView {
       public InternalSurfaceView(Context context) {
            super(context);
            setFocusableInTouchMode(true);
        }


    }

    @Override
    protected void onDestroy() {
        Log.v(TAG, "onDestroy");
        super.onDestroy();
        android.os.Process.killProcess(android.os.Process.myPid());
    }

    @Override
    protected void onPause() {
        Log.v(TAG, "onPause");
        super.onPause();
    }

    @Override
    protected void onResume() {
        Log.v(TAG, "onResume");
        super.onResume();
    }

    @Override
    protected void onStart() {
        Log.v(TAG, "onStart");
        super.onStart();
    }

    @Override
    protected void onRestart() {
        Log.v(TAG, "onRestart");
        super.onRestart();
    }

    @Override
    protected void onStop() {
        Log.v(TAG, "onStop");
        super.onStop();
    }


}
