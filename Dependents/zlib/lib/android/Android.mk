LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := voidengine_libz
LOCAL_MODULE_FILENAME := zlib
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libz.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../include
include $(PREBUILT_STATIC_LIBRARY)