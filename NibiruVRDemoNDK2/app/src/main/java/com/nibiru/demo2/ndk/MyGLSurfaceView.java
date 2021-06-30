package com.nibiru.demo2.ndk;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.SurfaceHolder;

import com.nibiru.lib.vr.NibiruVR;

public class MyGLSurfaceView extends GLSurfaceView{

	static{
		System.loadLibrary("test_ndk");
	}
	
	boolean surfaceHasDestroyed = false;
	public MyGLSurfaceView(Context context, Activity activity) {
		
		super(context);
		Log.v("ndk", "========================MyGLSurfaceView()");
		setEGLContextClientVersion(2);
		setEGLConfigChooser(8, 8, 8, 8, 16, 0);

		NibiruVR.destroyNibiruVRService();
		NibiruVR.clearBeforeInit();

		initializeNative(this, activity, context.getAssets());
		
		setRenderer(new Renderer() {
			
			@Override
			public void onSurfaceCreated(GL10 gl, EGLConfig config) {
				Log.v("ndk", "======================== onSurfaceCreated");
				onSurfaceCreatedNative();
			}
			
			@Override
			public void onSurfaceChanged(GL10 gl, int width, int height) {
				// TODO Auto-generated method stub
				Log.v("ndk", "======================== on surface changed: " + width + " : " + height);
				if (surfaceHasDestroyed) {
					onSurfaceCreatedNative();
					surfaceHasDestroyed = false;
				}
				onSurfaceChangedNative(width, height);
			}
			
			@Override
			public void onDrawFrame(GL10 gl) {
				// TODO Auto-generated method stub
				onDrawFrameNative();
			}
		});
	}
	

	@Override
	protected void onAttachedToWindow() {
		// TODO Auto-generated method stub
		super.onAttachedToWindow();
		
		onAttachWindowNative();
		Log.v("ndk", "========================onAttachedToWindow");
	}

	@Override
	protected void onDetachedFromWindow() {
		// TODO Auto-generated method stub
		super.onDetachedFromWindow();
		onDetachWindowNative();
		Log.v("ndk", "========================onDetachedFromWindow");
	}
	
	
	@Override
	public void onPause() {
		// TODO Auto-generated method stub
		onPauseNative();
		super.onPause();
		Log.v("ndk", "========================onPause");
	}


	@Override
	public void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
		onResumeNative();
		Log.v("ndk", "========================onResume");
	}

	@Override
	public void surfaceCreated(SurfaceHolder holder) {
		super.surfaceCreated(holder);
		Log.v("ndk", "========================surfaceCreated");
	}

	@Override
	public void surfaceDestroyed(SurfaceHolder holder) {
		queueEvent(new Runnable() {

			@Override
			public void run() {
				Log.v("ndk", "GL Runnable : surfaceDestroyed");
				surfaceHasDestroyed = true;
				onSurfaceDestroyed();
			}
		});
		super.surfaceDestroyed(holder);
		Log.v("ndk", "========================surfaceDestroyed");
	}

	native void initializeNative(GLSurfaceView view, Activity activity, AssetManager assets);
	
	native void onSurfaceCreatedNative();
	
	native void onSurfaceChangedNative(int width, int height);
	
	native void onDrawFrameNative();
	
	native void onAttachWindowNative();
	
	native void onDetachWindowNative();
	
	native void onPauseNative();
	
	native void onResumeNative();

	native int onKeyDownNative(int keyCode);

	native void onSurfaceDestroyed();
}
