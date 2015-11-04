LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := voidengine_libopenexr
LOCAL_MODULE_FILENAME := libopenexr
LOCAL_SRC_FILES := $(TARGET_ARCH_ABI)/libopenexr.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../include
include $(PREBUILT_STATIC_LIBRARY)
