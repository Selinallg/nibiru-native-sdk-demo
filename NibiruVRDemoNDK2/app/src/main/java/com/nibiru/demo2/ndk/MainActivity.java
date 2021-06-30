package com.nibiru.demo2.ndk;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Window;

import com.nibiru.lib.vr.NibiruVR;

public class MainActivity extends Activity {

    private static final String TAG = "MainActivity";

    MyGLSurfaceView mView;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(TAG, "onCreate: ");
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        super.onCreate(savedInstanceState);
        mView = new MyGLSurfaceView(this, this);

        setContentView(mView);
    }

    @Override
    public void onPause() {
        Log.d(TAG, "onPause: ");
        // TODO Auto-generated method stub
        if (mView != null) {
            mView.onPause();
        }
        super.onPause();


    }

    @Override
    public void onResume() {
        Log.d(TAG, "onResume: ");
        // TODO Auto-generated method stub
        super.onResume();

        if (mView != null) {
            mView.onResume();
        }


        //gotoSecondActivity();

    }

    private void gotoSecondActivity() {
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                Log.d(TAG, "run: gotoSecondActivity");
                Intent intent = new Intent(MainActivity.this, ThirdActivity.class);
                startActivity(intent);
                finish();
            }
        }, 1000 * 10);

    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        // TODO Auto-generated method stub

        //Let NDK handle key event
        if (mView != null) {
            if (mView.onKeyDownNative(keyCode) == 1) {
                return true;
            }
        }

        return super.onKeyDown(keyCode, event);
    }

    @Override
    protected void onDestroy() {
        Log.d(TAG, "onDestroy: ");
        super.onDestroy();
//        if (NibiruVR.getUsingNibiruVRServiceGL() != null) {
//            NibiruVR.getUsingNibiruVRServiceGL().destory();
//            NibiruVR.destroyNibiruVRService();
//        }
        // System.exit(0);
    }


}
