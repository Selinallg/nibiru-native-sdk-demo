LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := test_ndk
LOCAL_SRC_FILES := test_ndk.cpp \
World.cpp \
WorldMultiView.cpp \

LOCAL_LDLIBS    := -llog -lGLESv3 -lEGL -lm -landroid
LOCAL_STATIC_LIBRARIES := cpufeatures android_native_app_glue

#add nvr_helper
LOCAL_SHARED_LIBRARIES := nvr_ndk_helper

include $(BUILD_SHARED_LIBRARY)
#include nvr helper mk file
include $(LOCAL_PATH)/prebuilt/nvr_ndk_helper.mk


