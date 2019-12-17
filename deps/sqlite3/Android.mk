# File: Android.mk
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := sqlite3
LOCAL_MODULE_FILENAME := libsqlite
LOCAL_SRC_FILES := ./lib/$(TARGET_ARCH_ABI)/libsqlite.a

include $(PREBUILT_STATIC_LIBRARY)

