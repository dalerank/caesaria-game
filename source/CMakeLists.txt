project(CaesarIA-game)

# Find revision ID and hash of the sourcetree
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/GenerateVersionHPP.cmake")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/" ${CMAKE_MODULE_PATH})

aux_source_directory(. SRC_LIST)
file(GLOB INC_LIST "*.hpp")

find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_ttf REQUIRED)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${SDL_INCLUDE_DIR}
  ${SDL_MIXER_INCLUDE_DIR}
  ${SDL_TTF_INCLUDE_DIR}
  ${DEP_SOURCE_DIR}
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
file(GLOB GAME_MODELS_LIST "${GAME_CONFIG_DIR}/game/*.model")
file(GLOB GUI_MODELS_LIST "${GAME_CONFIG_DIR}/gui/*.gui")
file(GLOB TUTORIAL_MODELS_LIST "${GAME_CONFIG_DIR}/tutorial/*.tutorial")
file(GLOB MISSIONS_LIST "${GAME_CONFIG_DIR}/missions/*.*")
file(GLOB SCENES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/scene/*.*")
file(GLOB RELIGION_LIST "${CMAKE_CURRENT_SOURCE_DIR}/religion/*.*")

add_executable(${PROJECT_NAME} ${SRC_LIST} ${INC_LIST}
               ${GAME_MODELS_LIST} ${GUI_MODELS_LIST} ${EVENTS_SOURCES_LIST}
               ${CORE_SOURCES_LIST} ${GUI_SOURCES_LIST} ${WALKER_SOURCES_LIST}
               ${OBJECTS_SOURCES_LIST} ${GAME_SOURCES_LIST} ${VFS_SOURCES_LIST}
               ${PATHWAY_SOURCES_LIST} ${CITY_SOURCES_LIST} ${GOOD_SOURCES_LIST}
               ${GFX_SOURCES_LIST} ${SOURCES_LIST} ${SOUND_SOURCES_LIST} ${WORLD_SOURCES_LIST}
               ${MISSIONS_LIST} ${TUTORIAL_MODELS_LIST} ${SCENES_LIST} ${RELIGION_LIST})

target_link_libraries(${PROJECT_NAME}
  ${SDL_LIBRARY}
  ${SDL_MIXER_LIBRARY}
  ${SDL_TTF_LIBRARY}
)

#find_package( ZLIB )
#if( ZLIB_FOUND )
#  include_directories(${ZLIB_INCLUDE_DIRS})
#  target_link_libraries(${PROJECT_NAME} ${ZLIB_LIBRARIES})
#else
if( BUILD_ZLIB )
  link_directories(${ZLIB_HOME})
  include_directories(${ZLIB_HOME})
  target_link_libraries(${PROJECT_NAME} ${ZLIB_NAME})
endif()

if( BUILD_AES )
  link_directories(${AESLIB_HOME})
  target_link_libraries(${PROJECT_NAME} ${AESLIB_NAME})
endif()

#find_package( BZIP2 )
#if ( BZIP2_FOUND )
#  include_directories(${BZIP2_INCLUDE_DIRS})
#  target_link_libraries(${PROJECT_NAME} ${BZIP2_LIBRARIES})
#else
if( BUILD_BZIP )
  link_directories(${BZIPLIB_HOME})
  target_link_libraries(${PROJECT_NAME} ${BZIPLIB_NAME})
endif()

if( BUILD_LZMA )
  link_directories(${LZMALIB_HOME})
  target_link_libraries(${PROJECT_NAME} ${LZMALIB_NAME})
endif()

if( BUILD_SMK )
  link_directories(${SMKLIB_HOME})
  target_link_libraries(${PROJECT_NAME} ${SMKLIB_NAME})
endif()

#find_package( PNG )
#if (PNG_FOUND)
#  include_directories(${PNG_INCLUDE_DIRS})
#  target_link_libraries(${PROJECT_NAME} ${PNG_LIBRARIES})
#else
if( BUILD_PNG )
  link_directories(${PNGLIB_HOME})
  include_directories(${PNGLIB_HOME})
  target_link_libraries(${PROJECT_NAME} ${PNGLIB_NAME})
endif()

#if(OPENGL_RENDER)
if(WIN32)
  target_link_libraries(${PROJECT_NAME} "opengl32")
else()
  target_link_libraries(${PROJECT_NAME} "GL")
endif(WIN32)
#endif(OPENGL_RENDER)

if(WIN32)
  set(EXECUTABLE_FILENAME "caesaria")
endif(WIN32)

if(UNIX)
  set(EXECUTABLE_FILENAME "caesaria.linux")

  if(APPLE)
    set(EXECUTABLE_FILENAME "caesaria.macos")
  endif(APPLE)
endif(UNIX)

if(HAIKU)
  set(EXECUTABLE_FILENAME "caesaria.haiku")
endif(HAIKU)

set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME ${EXECUTABLE_FILENAME})

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  message("CaesarIA-game: building in release mode")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
else()
  message("CaesarIA-game: building in debug mode")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -g")
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

# set compiler options
if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  if(HAIKU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
  else(HAIKU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wno-unused-value")
  endif(HAIKU)
endif(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

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
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${GAME_CONFIG_DIR}/tutorial" "${WORK_DIR}/resources/tutorial"
)

ADD_CUSTOM_COMMAND(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/locale" "${WORK_DIR}/resources/locale"
)

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
    ${SDL_LIBRARY}
    ${SDL_MIXER_LIBRARY}
    ${SDL_TTF_LIBRARY}    
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
