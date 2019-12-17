LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := lua5.3
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/liblua5.3.a

include $(PREBUILT_STATIC_LIBRARY)
