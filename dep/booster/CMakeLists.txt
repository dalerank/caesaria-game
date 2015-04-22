project(booster)

add_definitions(-DSDL_LOADSO_DLOPEN)
add_definitions(-DSDL_VIDEO_OPENGL=1)
add_definitions(-DGL_GLEXT_PROTOTYPES)
add_definitions(-DHAVE_STDIO_H=1)
add_definitions(-DSDL_RENDER_DISABLED=0)
add_definitions(-DSDL_VIDEO_RENDER_OGL=1)
add_definitions(-DSDL_HAPTIC_DISABLED=1)
add_definitions(-DSDL_JOYSTICK_DISABLED=1)

include_directories(include)
file(GLOB BOOSTER_HEADERS_LIST "include/*.h")
file(GLOB BOOSTER_SOURCES_LIST "src/*.c")
file(GLOB VIDEO_SOURCES_LIST "src/video/*.c")
file(GLOB TIMER_SOURCES_LIST "src/timer/*.c")
file(GLOB EVENTS_SOURCES_LIST "src/events/*.c")
file(GLOB FILE_SOURCES_LIST "src/file/*.c")
file(GLOB RENDER_SOURCES_LIST "src/render/*.c")
file(GLOB RENDER_SW_SOURCES_LIST "src/render/software/*.c")
file(GLOB STDLIB_SOURCES_LIST "src/stdlib/*.c")
file(GLOB LIBM_SOURCES_LIST "src/libm/*.c")
file(GLOB THREAD_SOURCES_LIST "src/thread/*.c")
file(GLOB AUDIO_SOURCES_LIST "src/audio/*.c")
file(GLOB AUDIO_DUMMY_SOURCES_LIST "src/audio/dummy/*.c")
file(GLOB VIDEO_DUMMY_SOURCES_LIST "src/video/dummy/*.c")
file(GLOB ATOMIC_SOURCES_LIST "src/atomic/*.c")
file(GLOB RENDER_OPENGL_SOURCES_LIST "src/render/opengl/*.c")
file(GLOB CPUINFO_SOURCES_LIST "src/cpuinfo/*.c")

if(UNIX AND NOT APPLE)

  file(GLOB TIMER_UNIX_SOURCES_LIST "src/timer/unix/*.c")
  file(GLOB VIDEO_X11_SOURCES_LIST "src/video/x11/*.c")
  file(GLOB PTHREAD_SOURCES_LIST "src/thread/pthread/*.c")
  file(GLOB DLOPEN_SOURCES_LIST "src/loadso/dlopen/*.c")
  file(GLOB AUDIO_ALSA_SOURCES_LIST "src/audio/alsa/*.c")
  file(GLOB FILESYSTEM_SOURCES_LIST "src/filesystem/unix/*.c")

  add_definitions(-DSDL_TIMER_UNIX)
  add_definitions(-DSDL_VIDEO_DRIVER_X11)
  add_definitions(-DSDL_VIDEO_DRIVER_X11_SUPPORTS_GENERIC_EVENTS)
  add_definitions(-DSDL_VIDEO_OPENGL_GLX=1)
  add_definitions(-DSDL_THREAD_PTHREAD=1)

  add_library(  ${PROJECT_NAME} STATIC
                ${BOOSTER_SOURCES_LIST}
                ${TIMER_SOURCES_LIST}
                ${VIDEO_SOURCES_LIST}
                ${TIMER_UNIX_SOURCES_LIST}
                ${EVENTS_SOURCES_LIST}
                ${RENDER_SOURCES_LIST}
                ${FILESYSTEM_SOURCES_LIST}
                ${FILE_SOURCES_LIST}
                ${AUDIO_SOURCES_LIST}
                ${STDLIB_SOURCES_LIST}
                ${ATOMIC_SOURCES_LIST}
                ${THREAD_SOURCES_LIST}
                ${PTHREAD_SOURCES_LIST}
                ${LIBM_SOURCES_LIST}
                ${AUDIO_ALSA_SOURCES_LIST}
                ${AUDIO_DUMMY_SOURCES_LIST}
                ${VIDEO_DUMMY_SOURCES_LIST}
                ${CPUINFO_SOURCES_LIST}
                ${RENDER_OPENGL_SOURCES_LIST}
                ${RENDER_SW_SOURCES_LIST}
                ${VIDEO_X11_SOURCES_LIST}
                ${DLOPEN_SOURCES_LIST}
              )

  # Required on Unix OS family to be able to be linked into shared libraries.
  set_target_properties(${PROJECT_NAME}
                        PROPERTIES POSITION_INDEPENDENT_CODE ON)

  target_link_libraries( ${PROJECT_NAME} pthread Xext X11 GL GLU)
endif()

if(WIN32)
  file(GLOB TIMER_WIN_SOURCES_LIST "src/timer/windows/*.c")
  file(GLOB VIDEO_WIN_SOURCES_LIST "src/video/windows/*.c")
  file(GLOB CORE_WIN_SOURCES_LIST "src/core/windows/*.c")
  file(GLOB AUDIO_WIN_SOURCES_LIST "src/audio/winmm/*.c")
  file(GLOB DLOPEN_WIN_SOURCES_LIST "src/loadso/windows/*.c")
  file(GLOB THREAD_WIN_SOURCES_LIST "src/thread/windows/*.c")

  add_library(  ${PROJECT_NAME} STATIC
                ${BOOSTER_SOURCES_LIST}
                ${VIDEO_SOURCES_LIST}
                ${TIMER_SOURCES_LIST}
                ${TIMER_WIN_SOURCES_LIST}
                ${STDLIB_SOURCES_LIST}
                ${VIDEO_WIN_SOURCES_LIST}
                ${EVENTS_SOURCES_LIST}
                ${FILE_SOURCES_LIST}
                ${RENDER_SOURCES_LIST}
                ${RENDER_SW_SOURCES_LIST}
                ${LIBM_SOURCES_LIST}
                ${CORE_WIN_SOURCES_LIST}
                ${THREAD_WIN_SOURCES_LIST}
                ${THREAD_SOURCES_LIST}
                ${AUDIO_SOURCES_LIST}
                ${AUDIO_DUMMY_SOURCES_LIST}
                ${VIDEO_DUMMY_SOURCES_LIST}
                ${ATOMIC_SOURCES_LIST}
                ${RENDER_OPENGL_SOURCES_LIST}
                ${AUDIO_WIN_SOURCES_LIST}
                ${DLOPEN_WIN_SOURCES_LIST}
                ${CPUINFO_SOURCES_LIST}
              )

   target_link_libraries( ${PROJECT_NAME} Imm32 opengl32 Version)
endif()

# Expose public includes to other through cache variable.
set(BOOSTER_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

set(BOOSTER_LIBRARY ${PROJECT_NAME}
    CACHE INTERNAL "${PROJECT_NAME}: Library Name" FORCE)
