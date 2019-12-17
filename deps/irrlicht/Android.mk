# File: Android.mk
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := Irrlicht
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libIrrlicht.a

LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(PREBUILT_STATIC_LIBRARY)

$(call import-module,android/native_app_glue)