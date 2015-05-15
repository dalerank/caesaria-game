LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := freetype

APP_SUBDIRS := $(patsubst $(LOCAL_PATH)/%, %, $(shell find $(LOCAL_PATH)/src -type d))

LOCAL_C_INCLUDES := $(foreach D, $(APP_SUBDIRS), $(LOCAL_PATH)/$(D)) $(LOCAL_PATH)/include
LOCAL_CFLAGS := -Os -DFT2_BUILD_LIBRARY 

LOCAL_CPP_EXTENSION := .cpp

LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,, \
                   $(wildcard $(LOCAL_PATH)/src/cff/*.c) \
                   $(wildcard $(LOCAL_PATH)/src/type1/*.c)\
                   $(wildcard $(LOCAL_PATH)/src/autofit/*.c)\
                   $(wildcard $(LOCAL_PATH)/src/cid/*.c)\
                   $(wildcard $(LOCAL_PATH)/src/pfr/*.c)\
                   $(wildcard $(LOCAL_PATH)/src/type42/*.c)\
                   $(wildcard $(LOCAL_PATH)/src/pcf/*.c)\
                   $(wildcard $(LOCAL_PATH)/src/psaux/*.c)\
                   $(wildcard $(LOCAL_PATH)/src/base/*.c)\
                   $(wildcard $(LOCAL_PATH)/src/pshinter/*.c)\
                   $(wildcard $(LOCAL_PATH)/src/raster/*.c)\
                   $(wildcard $(LOCAL_PATH)/src/sfnt/*.c)\
                   $(wildcard $(LOCAL_PATH)/src/smooth/*.c)\
                   $(wildcard $(LOCAL_PATH)/src/truetype/*.c) \
                   $(wildcard $(LOCAL_PATH)/src/bdf/*.c) \
                   $(LOCAL_PATH)/src/winfonts/winfnt.c \
                   $(LOCAL_PATH)/src/gzip/ftgzip.c \
                   $(LOCAL_PATH)/src/lzw/ftlzw.c \
                   $(LOCAL_PATH)/src/psnames/psmodule.c )

LOCAL_SHARED_LIBRARIES := 

LOCAL_STATIC_LIBRARIES := 

LOCAL_LDLIBS :=

include $(BUILD_STATIC_LIBRARY)
