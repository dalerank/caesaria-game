project(CaesarIA-game)

# Find revision ID and hash of the sourcetree
include("${CMAKE_MODULE_PATH}/GenerateVersionHPP.cmake")

aux_source_directory(. SRC_LIST)
file(GLOB INC_LIST "*.hpp")

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${SDL2_INCLUDE_DIR}
  ${SDL2_MIXER_INCLUDE_DIR}
  ${DEP_SOURCE_DIR}
  ${STEAM_API_DIR}
  ${STEAM_API_DIR}/public/opensteam
)

file(GLOB EVENTS_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/events/*.*")
file(GLOB CORE_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/core/*.*")
file(GLOB PATHWAY_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/pathway/*.*")
file(GLOB GUI_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/gui/*.*")
file(GLOB GAME_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/game/*.*")
file(GLOB CITY_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/city/*.*")
file(GLOB GOOD_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/good/*.*")
file(GLOB VFS_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/vfs/*.*")
file(GLOB GFX_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/gfx/*.*")
file(GLOB SOUND_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/sound/*.*")
file(GLOB SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/*.*")
file(GLOB OBJECTS_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/objects/*.*")
file(GLOB WALKER_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/walker/*.*")
file(GLOB WORLD_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/world/*.*")
file(GLOB GAME_MODELS_LIST "${GAME_CONFIG_DIR}/game/*.*")
file(GLOB GUI_MODELS_LIST "${GAME_CONFIG_DIR}/gui/*.gui")
file(GLOB TUTORIAL_MODELS_LIST "${GAME_CONFIG_DIR}/tutorial/*.tutorial")
file(GLOB MISSIONS_LIST "${GAME_CONFIG_DIR}/missions/*.*")
file(GLOB HELP_LIST "${GAME_CONFIG_DIR}/help/*.*")
file(GLOB SHADERS_LIST "${GAME_CONFIG_DIR}/shaders/*.*")
file(GLOB SCENES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/scene/*.*")
file(GLOB RELIGION_LIST "${CMAKE_CURRENT_SOURCE_DIR}/religion/*.*")
file(GLOB STEAM_SOURCES_LIST "${STEAM_API_DIR}/*.*")

add_executable(${PROJECT_NAME} ${SRC_LIST} ${INC_LIST}
               ${GAME_MODELS_LIST} ${GUI_MODELS_LIST} ${EVENTS_SOURCES_LIST}
               ${CORE_SOURCES_LIST} ${GUI_SOURCES_LIST} ${WALKER_SOURCES_LIST}
               ${OBJECTS_SOURCES_LIST} ${GAME_SOURCES_LIST} ${VFS_SOURCES_LIST}
               ${PATHWAY_SOURCES_LIST} ${CITY_SOURCES_LIST} ${GOOD_SOURCES_LIST}
               ${GFX_SOURCES_LIST} ${SOURCES_LIST} ${SOUND_SOURCES_LIST} ${WORLD_SOURCES_LIST}
               ${MISSIONS_LIST} ${TUTORIAL_MODELS_LIST} ${SCENES_LIST} ${RELIGION_LIST} ${SHADERS_LIST}
               ${STEAM_SOURCES_LIST} ${HELP_LIST} )

target_link_libraries(${PROJECT_NAME}
  ${SDL2_LIBRARY}
  ${SDL2_MIXER_LIBRARY}
)

include_directories(${ZLIB_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARY})

include_directories(${AES_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${AES_LIBRARY})

include_directories(${BZIP_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${BZIP_LIBRARY})

include_directories(${LZMA_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${LZMA_LIBRARY})

include_directories(${SMK_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SMK_LIBRARY})

include_directories(${SDL2_TTF_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SDL2_TTF_LIBRARY})

include_directories(${PNG_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${PNG_LIBRARY})

if(BUILD_GL_FRAMEBUFFER)
add_definitions( -DCAESARIA_USE_FRAMEBUFFER )
add_definitions( -DCAESARIA_USE_SHADERS )
endif(BUILD_GL_FRAMEBUFFER)

if( DEBUG_TIMERS )
add_definitions( -DCAESARIA_USE_DEBUGTIMERS )
endif(DEBUG_TIMERS)

if(WIN32)
  target_link_libraries(${PROJECT_NAME} "opengl32")
  target_link_libraries(${PROJECT_NAME} "winmm")
  set(EXECUTABLE_FILENAME "caesaria")
endif(WIN32)

if(UNIX)
  set(EXECUTABLE_FILENAME "caesaria.linux")
  if(NOT APPLE)
    target_link_libraries(${PROJECT_NAME} "GL")
    target_link_libraries(${PROJECT_NAME} "dl")
  endif()  
endif(UNIX)

if(USE_STEAM)
  if(WIN32)
    target_link_libraries(${PROJECT_NAME} "${STEAM_API_DIR}/bin/win/steam_api.lib")
    target_link_libraries(${PROJECT_NAME} "${STEAM_API_DIR}/helper/steam_helper.lib")
  elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} "${STEAM_API_DIR}/bin/osx32/libsteam_api.dylib")
  elseif(UNIX)
    if(X64_BUILD)
      target_link_libraries(${PROJECT_NAME} "${STEAM_API_DIR}/bin/linux64/libsteam_api.so")
    else()
      target_link_libraries(${PROJECT_NAME} "${STEAM_API_DIR}/bin/linux32/libsteam_api.so")
    endif()
  endif()
endif(USE_STEAM)

if(APPLE)
  set(EXECUTABLE_FILENAME "caesaria.macos")
  INCLUDE_DIRECTORIES ( /System/Library/Frameworks )
  FIND_LIBRARY(OpenGL_LIBRARY OpenGL )
  target_link_libraries(${PROJECT_NAME} ${OpenGL_LIBRARY} )
endif(APPLE)

if(HAIKU)
  set(EXECUTABLE_FILENAME "caesaria.haiku")
endif(HAIKU)

set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME ${EXECUTABLE_FILENAME})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  message("CaesarIA-game: building in release mode")
  if(NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
  endif()
else()
  message("CaesarIA-game: building in debug mode")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -g")
  add_definitions( -DDEBUG )
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  # using Clang
  add_definitions("-DCAESARIA_COMPILER_NAME=\"clang\"")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using GCC
  add_definitions("-DCAESARIA_COMPILER_NAME=\"gcc\"")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C++
  add_definitions("-DCAESARIA_COMPILER_NAME=\"icc\"" )
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
  add_definitions("-DCAESARIA_COMPILER_NAME=\"msvc\"")
endif()

message( "C++ compiler is ${CMAKE_CXX_COMPILER_ID}" )

# set compiler options
if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  if(HAIKU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  else(HAIKU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wno-unused-value")
  endif(HAIKU)
endif()

# tests for C++11 suport
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
if(COMPILER_SUPPORTS_CXX11)
  if(USE_CPP11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  endif()
else()
  if(USE_CPP11)
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Use proper compiler or disable USE_CPP11 option.")
  else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
  endif()
endif()

if(WIN32)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
  #set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -static-libgcc -static-libstdc++ -static")
endif(WIN32)

# prepare working directory
set(GAME_BINARY_FILENAME ${EXECUTABLE_FILENAME})
if(WIN32)
  set(GAME_BINARY_FILENAME ${GAME_BINARY_FILENAME}.exe)
endif(WIN32)

ADD_CUSTOM_COMMAND(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${GAME_BINARY_FILENAME}" "${WORK_DIR}/${GAME_BINARY_FILENAME}"
)

ADD_CUSTOM_COMMAND(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${GAME_CONFIG_DIR}/game" "${WORK_DIR}/resources"
)

ADD_CUSTOM_COMMAND(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${GAME_CONFIG_DIR}/gui" "${WORK_DIR}/resources/gui"
)

ADD_CUSTOM_COMMAND(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${GAME_CONFIG_DIR}/missions" "${WORK_DIR}/resources/missions"
)

ADD_CUSTOM_COMMAND(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${GAME_CONFIG_DIR}/help" "${WORK_DIR}/resources/help"
)

ADD_CUSTOM_COMMAND(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${GAME_CONFIG_DIR}/tutorial" "${WORK_DIR}/resources/tutorial"
)

ADD_CUSTOM_COMMAND(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${GAME_CONFIG_DIR}/maps" "${WORK_DIR}/resources/maps"
)

ADD_CUSTOM_COMMAND(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/locale" "${WORK_DIR}/resources/locale"
)

ADD_CUSTOM_COMMAND(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${GAME_CONFIG_DIR}/shaders" "${WORK_DIR}/resources/shaders"
)

if(USE_STEAM)
  if(WIN32)
    ADD_CUSTOM_COMMAND(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        #COMMAND ${CMAKE_COMMAND} -E copy "${STEAM_API_DIR}/bin/win/sdkencryptedappticket.dll" "${WORK_DIR}/sdkencryptedappticket.dll"
        COMMAND ${CMAKE_COMMAND} -E copy "${STEAM_API_DIR}/bin/win/steam_api.dll" "${WORK_DIR}/steam_api.dll"
        COMMAND ${CMAKE_COMMAND} -E copy "${STEAM_API_DIR}/helper/steam_helper.dll" "${WORK_DIR}/steam_helper.dll"
    )
  elseif(APPLE)
    ADD_CUSTOM_COMMAND(
        TARGET ${PROJECT_NAME}
        POST_BUILD
        #COMMAND ${CMAKE_COMMAND} -E copy "${STEAM_API_DIR}/bin/osx32/libsdkencryptedappticket.dylib" "${WORK_DIR}/libsdkencryptedappticket.dylib"
        COMMAND ${CMAKE_COMMAND} -E copy "${STEAM_API_DIR}/bin/osx32/libsteam_api.dylib" "${WORK_DIR}/libsteam_api.dylib"
    )
  elseif(UNIX)
    if(X64_BUILD)
      ADD_CUSTOM_COMMAND(
          TARGET ${PROJECT_NAME}
          POST_BUILD
          #COMMAND ${CMAKE_COMMAND} -E copy "${STEAM_API_DIR}/bin/linux64/libsdkencryptedappticket.so" "${WORK_DIR}/libsdkencryptedappticket.so"
          COMMAND ${CMAKE_COMMAND} -E copy "${STEAM_API_DIR}/bin/linux64/libsteam_api.so" "${WORK_DIR}/libsteam_api.so"
      )
    else()
      ADD_CUSTOM_COMMAND(
          TARGET ${PROJECT_NAME}
          POST_BUILD
          #COMMAND ${CMAKE_COMMAND} -E copy "${STEAM_API_DIR}/bin/linux32/libsdkencryptedappticket.so" "${WORK_DIR}/libsdkencryptedappticket.so"
          COMMAND ${CMAKE_COMMAND} -E copy "${STEAM_API_DIR}/bin/linux32/libsteam_api.so" "${WORK_DIR}/libsteam_api.so"
     )
    endif()
  endif()
endif()

if(EXT_BINDIR)
  message( "game: ext bindir=${CMAKE_SOURCE_DIR}/${EXT_BINDIR}")
  ADD_CUSTOM_COMMAND(
      TARGET ${PROJECT_NAME}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${GAME_BINARY_FILENAME}" "${CMAKE_SOURCE_DIR}/${EXT_BINDIR}/${GAME_BINARY_FILENAME}"
  )
endif(EXT_BINDIR)



# Copy DLL to build output directory
if(WIN32)
  set(_LIBS_FOR_DLL_SOURCE
    ${SDL2_LIBRARY}
    ${SDL2_MIXER_LIBRARY}
    ${SDL2_TTF_LIBRARY}
  )

  foreach(_lib ${_LIBS_FOR_DLL_SOURCE})
    GET_FILENAME_COMPONENT(_lib_abs ${_lib} ABSOLUTE)
    GET_FILENAME_COMPONENT(_lib_path ${_lib} PATH)

    # Gather list of all .xml and .conf files in "/config"
    file(GLOB DLLs ${_lib_path}/*.dll
      ${_lib_path}/../bin/*.dll)

    foreach(dll ${DLLs})
      add_custom_command( TARGET ${PROJECT_NAME}
                          POST_BUILD
                          COMMAND ${CMAKE_COMMAND} -E copy ${dll} ${WORK_DIR})
    endforeach()
  endforeach(_lib)
endif(WIN32)
