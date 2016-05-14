LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := litehtml

APP_SUBDIRS := $(patsubst $(LOCAL_PATH)/%, %, $(shell find $(LOCAL_PATH)/src -type d))

LOCAL_C_INCLUDES := $(foreach D, $(APP_SUBDIRS), $(LOCAL_PATH)/$(D)) $(LOCAL_PATH)/include
LOCAL_CFLAGS := -Os

LOCAL_CPP_EXTENSION := .cpp

LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,, \
                   $(wildcard $(LOCAL_PATH)/src/*.c))

LOCAL_SHARED_LIBRARIES := 

LOCAL_STATIC_LIBRARIES := 

LOCAL_LDLIBS :=

include $(BUILD_STATIC_LIBRARY)
