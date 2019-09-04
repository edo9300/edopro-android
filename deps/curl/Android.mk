# File: Android.mk
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := curl
LOCAL_MODULE_FILENAME := libcurl
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libcurl.a

include $(PREBUILT_STATIC_LIBRARY)
