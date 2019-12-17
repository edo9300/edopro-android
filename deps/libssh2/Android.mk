LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ssh2
LOCAL_MODULE_FILENAME := libssh2
LOCAL_STATIC_LIBRARIES := libssl_static
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libssh2.a

include $(PREBUILT_STATIC_LIBRARY)
