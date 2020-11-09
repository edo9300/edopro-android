# File: Android.mk
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := Irrlicht
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libIrrlicht.a

LOCAL_STATIC_LIBRARIES := android_native_app_glue

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_EXPORT_LDLIBS := -lEGL -llog -lGLESv1_CM -lGLESv2 -landroid

include $(PREBUILT_STATIC_LIBRARY)

$(call import-module,android/native_app_glue)