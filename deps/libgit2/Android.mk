LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := git2
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libgit2.a

include $(PREBUILT_STATIC_LIBRARY)
