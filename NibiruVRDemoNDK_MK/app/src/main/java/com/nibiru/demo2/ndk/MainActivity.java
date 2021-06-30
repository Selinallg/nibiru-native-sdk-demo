package com.nibiru.demo2.ndk;

import android.app.Activity;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.Window;

import com.nibiru.lib.vr.NibiruVR;

public class MainActivity extends Activity {

	MyGLSurfaceView mView;
	

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		
		super.onCreate(savedInstanceState);
		mView = new MyGLSurfaceView(this, this);
		
		setContentView(mView);
	}
	
	@Override
	public void onPause() {
		// TODO Auto-generated method stub
		if(mView != null){
			mView.onPause();
		}
		super.onPause();
		

	}
	
	@Override
	public void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
		
		if(mView != null){
			mView.onResume();
		}

	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		
		//Let NDK handle key event
		if( mView != null  ){
			if( mView.onKeyDownNative(keyCode) == 1 ){
				return true;
			}
		}
		
		return  super.onKeyDown(keyCode, event);
	}

	@Override
	protected void onDestroy() {
		super.onDestroy();
		if (NibiruVR.getUsingNibiruVRServiceGL() != null) {
			NibiruVR.getUsingNibiruVRServiceGL().destory();
			NibiruVR.destroyNibiruVRService();
		}
		// System.exit(0);
	}
	
	

}
