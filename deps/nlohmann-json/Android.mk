LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := nlohmann-json

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_EXPORT_CFLAGS := -DJSON_HAS_CPP_14

include $(BUILD_STATIC_LIBRARY)

# version 3.10.4