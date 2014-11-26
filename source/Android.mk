LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := application

SDL_PATH       := ../../libs/SDL
SDL_MIXER_PATH := ../../libs/SDL_mixer
SDL_NET_PATH   := ../../libs/SDL_net
STEAM_PATH     := ../steam
GAME_PATH := $(LOCAL_PATH)
DEP_PATH := ../dep

LOCAL_C_INCLUDES := \
  $(LOCAL_PATH)/$(SDL_PATH)/include \
  $(LOCAL_PATH)/$(SDL_MIXER_PATH) \
  $(LOCAL_PATH)/$(SDL_NET_PATH)/include \
  $(LOCAL_PATH)/$(FREETYPE_PATH)/include \
  $(LOCAL_PATH)/$(GAME_PATH) \
  $(LOCAL_PATH)/$(DEP_PATH) \
  $(LOCAL_PATH)/$(DEP_PATH)/ttf \
  $(LOCAL_PATH)/$(DEP_PATH)/lzma \
  $(LOCAL_PATH)/$(DEP_PATH)/bzip2 \
  $(LOCAL_PATH)/$(DEP_PATH)/zlib \
  $(LOCAL_PATH)/$(DEP_PATH)/aes \
  $(LOCAL_PATH)/$(DEP_PATH)/smk \
  $(LOCAL_PATH)/$(STEAM_PATH) \
  $(LOCAL_PATH)/$(DEP_PATH)/libpng

# Add your application source files here...
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,, \
  $(wildcard $(GAME_PATH)/*.cpp) \
  $(wildcard $(GAME_PATH)/core/*.cpp) \
  $(wildcard $(GAME_PATH)/vfs/*.cpp) \
  $(wildcard $(GAME_PATH)/objects/*.cpp) \
  $(wildcard $(GAME_PATH)/gui/*.cpp) \
  $(wildcard $(GAME_PATH)/city/*.cpp) \
  $(wildcard $(GAME_PATH)/gfx/*.cpp) \
  $(wildcard $(GAME_PATH)/events/*.cpp) \
  $(wildcard $(GAME_PATH)/world/*.cpp) \
  $(wildcard $(GAME_PATH)/pathway/*.cpp) \
  $(wildcard $(GAME_PATH)/walker/*.cpp) \
  $(wildcard $(GAME_PATH)/good/*.cpp) \
  $(wildcard $(GAME_PATH)/religion/*.cpp) \
  $(wildcard $(GAME_PATH)/scene/*.cpp) \
  $(wildcard $(GAME_PATH)/sound/*.cpp) \
  $(wildcard $(GAME_PATH)/game/*.cpp))
  
LOCAL_SHARED_LIBRARIES := SDL2 SDL2_mixer SDL2_net sdl_ttf pnggo lzma bzip2 aes smk
LOCAL_CPP_FEATURES += exceptions
LOCAL_CPP_FEATURES += rtti
LOCAL_LDLIBS := -lGLESv1_CM -llog

include $(BUILD_SHARED_LIBRARY)
