LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := fmt_static
LOCAL_MODULE_FILENAME := libfmt
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libfmt.a

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

include $(PREBUILT_STATIC_LIBRARY)

# version 8.0.1