LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := zlib
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libz.a

include $(PREBUILT_STATIC_LIBRARY)
