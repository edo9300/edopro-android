LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := SDL2
LOCAL_SHARED_LIBRARIES := hidapi
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libSDL2.so

include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE := libhidapi
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libhidapi.so

include $(PREBUILT_SHARED_LIBRARY)
