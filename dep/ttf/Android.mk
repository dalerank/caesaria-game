LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../freetype/include \
                  $(LOCAL_PATH)/../../android/libs/SDL/include 

LOCAL_MODULE := sdl_ttf
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,, \
    $(wildcard $(LOCAL_PATH)/*.c))

LOCAL_SHARED_LIBRARIES := freetype SDL2 zlib

include $(BUILD_SHARED_LIBRARY)
