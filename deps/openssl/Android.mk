LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE:= libcrypto_static
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libcrypto_static.a

include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE:= libssl_static
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libssl_static.a

include $(PREBUILT_STATIC_LIBRARY)
