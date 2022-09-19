LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := AssetCopier
LOCAL_MODULE_FILENAME := libassetcopier
LOCAL_SRC_FILES :=  copy.c
LOCAL_CFLAGS := -Wpedantic
LOCAL_LDLIBS := -llog -landroid

include $(BUILD_SHARED_LIBRARY)

