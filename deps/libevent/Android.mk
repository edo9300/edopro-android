LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE:= libevent2
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libevent2.a

include $(PREBUILT_STATIC_LIBRARY)
