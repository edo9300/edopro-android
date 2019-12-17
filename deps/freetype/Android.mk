LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libft2
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libft2.a

include $(PREBUILT_STATIC_LIBRARY)
