LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libevent_pthreads
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libevent_pthreads.a

include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE:= libevent2
LOCAL_STATIC_LIBRARIES := libevent_pthreads
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libevent_core.a

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

include $(PREBUILT_STATIC_LIBRARY)
