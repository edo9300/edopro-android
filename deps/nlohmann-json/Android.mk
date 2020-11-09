LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := nlohmann-json

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

include $(BUILD_STATIC_LIBRARY)

