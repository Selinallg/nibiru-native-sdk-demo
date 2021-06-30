#include <jni.h>
#include "World.cpp"
#include "WorldMultiView.cpp"
#include "NibiruVRApi.h"

/*
 * * Nibiru VR SDK Usage
 *
 * * Nibiru VR SDK is for OpenGL NDK development.
 *
 * * >>> EASY usage and CUSTOM usage: reference to demo project in VR SDK package.
 * * >>> NDK usage: reference to /jni/test_ndk.cpp.
 *
 * 1. Check AndroidManifest.xml Add TAG <category android:name="com.nibiru.intent.category.NVR" /> with  <category android:name="android.intent.category.LAUNCHER" />
 * 2. Make sure Application is Full Screen, add theme in Application TAG:  android:theme="@android:style/Theme.NoTitleBar.Fullscreen"
 * 3. Add Meta-data <meta-data android:name="NibiruVRVersion" android:value="2" /> in TAG <application/>
 * 4. Custom usage requires to use GLSurfaceView to render.
 * 5. Init NibiruVRApi after creating GLSurfaceView (sample codes in this cpp).
 * 6. NibiruVRApi provides VR Apis
 *
 */

using namespace nvr;

//Nibiru VR Service, provide VR Apis
static NibiruVRApi sNVRApi;

//Test VR Scene
static NVRScene *sScene;

// is using multiview ?? it is just a flag
static bool isMultiView = false;

void onDrawMultiView(nvr_Eye *eyes) {
	if (sScene != NULL) {
		sScene->onDrawMultiView(eyes);
	}
}

void onDraw(nvr_Eye *eye) {
	if (sScene == NULL)
		return;

	//Each eye render
	for (int i = 0; i < EYE_NUM; i++) {
		nvr_Eye useEye = eye[i];

		//Prepare draw eye on one eye
		int result = sNVRApi.nvr_PrepareDrawEye(useEye.pos);

		if (result >= 0) {
			sScene->onDrawEye(&useEye);

			//Finish draw eye on one eye
			sNVRApi.nvr_FinishDrawEye(useEye.pos);
		}
	}
}

void onDrawFrame() {
	mat4 headpose;
	nvr_Eye eye[EYE_NUM];
	float position[3];
	memset(position, 0, sizeof(float) * 3);

	//Call prepare draw to get head info and check if can render now
	int res = sNVRApi.nvr_PrepareDraw2(&headpose, position, eye);

	//Not ready for render
	if (res < 0) {
		NLOGD("nvr api is not ready");
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0, 0, 0, 1);
		return;
	}

	sScene->onPreDraw(position);

	if (isMultiView) {
		onDrawMultiView(eye);
	} else {
		onDraw(eye);
	}

	// sNVRApi.nvr_setFramePose(headpose.value_ptr());
	//Finish this frame draw
	sNVRApi.nvr_FinishDraw();

	// test code
	//mat4 pose;
	//float predictiveTimeMS = 16;
	//sNVRApi.nvr_getPredictiveHeadPose(&pose, predictiveTimeMS);
	// test code
}

extern "C" {

JNIEXPORT void JNICALL Java_com_nibiru_demo2_ndk_MyGLSurfaceView_initializeNative(
		JNIEnv* env, jobject obj, jobject surfaceview, jobject activity, jobject assets) {

	NLOGD("initialized native");

	//Init NVR Api in construct method of GLSurfaceView
	int res = sNVRApi.nvr_Initialize(env, surfaceview, activity);

	if( res == 0 ) {
		NLOGD("init nvr succ");
	} else {
		NLOGE("init nvr failed");
	}

	//Check the Nibiru OS support DTR
	bool support = sNVRApi.isDTRSupport();

	NLOGD("Support DTR: %d", support);

	int sdkVer[3];
	sNVRApi.nvr_getSDKVersion(sdkVer);
	NLOGD("SDK Version : V%d,ReleaseDate.%d, Revision.%d", sdkVer[0], sdkVer[1], sdkVer[2]);

	bool useMultiView = false;

	//Check the System support Multi-View rendering.
	if( support && ( sNVRApi.nvr_IsSupportMultiView() || sNVRApi.nvr_IsSupportMultiView2()) && useMultiView) {
		//Recommend to use multi-view renderering, but at the same time, developer should support the normal VR renderering for compatiblity.
		NLOGD("use multi-view");
		sScene = new WorldMultiView();
		sNVRApi.nvr_SetEnableMultiView(true);
		isMultiView = true;
	} else {
		NLOGD("use normal");
		sScene = new World();
		isMultiView = false;

		if(sNVRApi.nvr_IsSupportDirectRender()) {
			NLOGD("enable direct render");
			sNVRApi.nvr_SetEnableDirectRender(true);
		} else {
			NLOGD("no use direct render");
		}
	}

	bool support6dof = sNVRApi.isSupport6Dof();
	NLOGD("Support 6DOF: %d", support6dof);
	sScene->setSupport6Dof(support6dof);
	//1=rotation.2=position
	sNVRApi.setTrackingMode(support6dof ? 2:1);
	sScene->setAssets(env, assets);
	sScene->setNVRApi(&sNVRApi);

	//Set Game Mode, which main top FPS rendering and HIGH GPU/CPU performance, but take MORE power consumption
//	sNVRApi.nvr_setGameMode(true);

//Set Display Quality, Better quality leads to MORE power consumption
//	sNVRApi.nvr_setDisplayQuality(NVR_DISPLAY_QUALITY_HIGH);

//Show FPS in logcat
//	sNVRApi.nvr_SetEnableFPS(true);

	NLOGD("init nvr service res: %d", res);

}

JNIEXPORT void JNICALL Java_com_nibiru_demo2_ndk_MyGLSurfaceView_onSurfaceCreatedNative(
		JNIEnv* env, jobject obj) {

	NLOGD("on surface created");

	//Notify NVR Api surface created
	sNVRApi.nvr_OnSurfaceCreated();

	if( sScene != NULL ) {
		sScene->onSceneCreated();
	}

}

JNIEXPORT void JNICALL Java_com_nibiru_demo2_ndk_MyGLSurfaceView_onSurfaceDestroyed(
		JNIEnv* env, jobject obj) {

	NLOGD("on surface destroyed");

	if( sScene != NULL ) {
		sScene->onSceneDestroyed();
	}

}

JNIEXPORT void JNICALL Java_com_nibiru_demo2_ndk_MyGLSurfaceView_onSurfaceChangedNative(
		JNIEnv* env, jobject obj, jint width, jint height) {

	NLOGD("on surface changed");
	//Notify NVR Api surface changed
	sNVRApi.nvr_OnSurfaceChanged(width, height);

	float values[2];
	sNVRApi.nvr_GetCurrentNearFar(values);
	//0: near; 1: far
	float near = values[0];
	float far = values[1];

	NLOGI("current near: %f, far: %f", near, far);

	//Only set FAR to 100
	sNVRApi.nvr_UpdateNearFar(near , 100);

}

JNIEXPORT void JNICALL Java_com_nibiru_demo2_ndk_MyGLSurfaceView_onDrawFrameNative(
		JNIEnv* env, jobject obj) {

//	NLOGD("on draw frame");

//Draw frame
	onDrawFrame();

}

JNIEXPORT void JNICALL Java_com_nibiru_demo2_ndk_MyGLSurfaceView_onDetachWindowNative(
		JNIEnv* env, jobject obj) {

	NLOGD("on detach");

	//Notify NVR Api window detached
	sNVRApi.nvr_DetachWindow();

	//Destor NVR Api
	sNVRApi.nvr_Destory();
	sScene->setNVRApi(NULL);
	delete(sScene);
	sScene = NULL;

}

JNIEXPORT void JNICALL Java_com_nibiru_demo2_ndk_MyGLSurfaceView_onAttachWindowNative(
		JNIEnv* env, jobject obj) {

	NLOGD("on attach");

	//Notify NVR Api attach window
	sNVRApi.nvr_AttachWindow();

}

JNIEXPORT void JNICALL Java_com_nibiru_demo2_ndk_MyGLSurfaceView_onPauseNative(
		JNIEnv* env, jobject obj) {

	NLOGD("on pause");
	//Notify NVR Api on pause
	sNVRApi.nvr_OnPause();

}

JNIEXPORT void JNICALL Java_com_nibiru_demo2_ndk_MyGLSurfaceView_onResumeNative(
		JNIEnv* env, jobject obj) {

	NLOGD("on resume");
	//notify NVR Api resume
	sNVRApi.nvr_OnResume();

}

//Test state flags
bool isEnableTracker = true;
bool is2D = false;
bool isHandleNKey = false;
bool isShowGaze = false;

JNIEXPORT jint JNICALL Java_com_nibiru_demo2_ndk_MyGLSurfaceView_onKeyDownNative(
		JNIEnv* env, jobject obj, jint keycode) {

	//called by onKeyDown, use nvr_GetInputKey to get input keycode
	int nvr_key = sNVRApi.nvr_GetInputKey(keycode);

	if (nvr_key == NVR_KEY_ENTER) {

		/* Test Gaze*/

		//Set point color to green, default is white
//		sNVRApi.nvr_SetGazeColor(0, 1, 0 );
//		sNVRApi.nvr_SetGazeDistance(-3);
//		sNVRApi.nvr_SetGazeSize(GAZE_SIZE_DEFAULT);
//		if( isShowGaze ){
//			sNVRApi.nvr_HideGaze();
//		}else{
//			sNVRApi.nvr_ShowGaze();
//		}
//
//		isShowGaze = !isShowGaze;
//Test Enable/Disable Tracker
		isEnableTracker = !isEnableTracker;

		if (isEnableTracker) {
			sNVRApi.nvr_unlockTracker();
		} else {

			//Test lock the tracker, after locking, the tracker will fix on the current position
			sNVRApi.nvr_lockTracker(NVR_LOCK_CUR);

			//Test lock the tracker and reset to the front.
//			sNVRApi.nvr_lockTracker(NVR_LOCK_FRONT);
		}

		//Test Enable/Disable System VR Mode
//		sNVRApi.nvr_SetSystemVRMode(is2D ? NVR_MODE_NVR : NVR_MODE_2D );
//		is2D = !is2D;

//Test Enable/Disable Reg N Key

//		if( isHandleNKey ){
//			sNVRApi.nvr_UnRegHandleNKey();
//		}else{
//			sNVRApi.nvr_RegHandleNKey();
//
//		}
//		isHandleNKey = !isHandleNKey;

		return 1;
	} else if (nvr_key == NVR_KEY_BACK) {
		NLOGD("on nvr key down: BACK");
	} else if (nvr_key == NVR_KEY_UP) {
		NLOGD("on nvr key down: UP");
	} else if (nvr_key == NVR_KEY_DOWN) {
		NLOGD("on nvr key down: DOWN");
	} else if (nvr_key == NVR_KEY_LEFT) {
		NLOGD("on nvr key down: LEFT");
	} else if (nvr_key == NVR_KEY_RIGHT) {
		NLOGD("on nvr key down: RIGHT");
	} else if (nvr_key == NVR_KEY_NKEY) {
		NLOGD("on nvr key down: NKEY");
	} else if (nvr_key == NVR_KEY_NONE) {
		NLOGD("on nvr key down: NONE");
	}

	return 0;

}

}
