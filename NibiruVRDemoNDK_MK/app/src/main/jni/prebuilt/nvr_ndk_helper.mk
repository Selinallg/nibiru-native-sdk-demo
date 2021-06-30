LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := nvr_ndk_helper

LOCAL_SRC_FILES :=  $(TARGET_ARCH_ABI)/libnvr_ndk_helper.so

include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := nvr_local

LOCAL_SRC_FILES :=  $(TARGET_ARCH_ABI)/libnvr_local.so

include $(PREBUILT_SHARED_LIBRARY)

