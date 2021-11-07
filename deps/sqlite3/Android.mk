LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := sqlite3
LOCAL_MODULE_FILENAME := libsqlite
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libsqlite.a

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include

include $(PREBUILT_STATIC_LIBRARY)

# version 3.36.0