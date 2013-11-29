project(CaesarIA-game)

# Find revision ID and hash of the sourcetree
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/GenerateVersionHPP.cmake")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/" ${CMAKE_MODULE_PATH})

aux_source_directory(. SRC_LIST)
file(GLOB INC_LIST "*.hpp")

find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(PNG REQUIRED)
find_package(zlib REQUIRED)

if(NOT BUILD_LIBINTL)
  find_package(Libintl REQUIRED)
  include_directories( ${LIBINTL_INCLUDE_DIR} )
  link_libraries( ${LIBINTL_LIBRARIES} )
endif()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${UTILS_SOURCE_DIR}
  ${SDL_INCLUDE_DIR}
  ${SDL_MIXER_INCLUDE_DIR}
  ${SDL_TTF_INCLUDE_DIR}
  ${PNG_INCLUDE_DIR}
)

link_libraries(
  ${SDL_LIBRARY}
  ${SDL_MIXER_LIBRARY}
  ${SDL_TTF_LIBRARY}
  ${PNG_LIBRARY}
)

set(UTILS_SRC_LIST)
if(NO_USE_SYSTEM_ZLIB)
  add_definitions(-DNO_USE_SYSTEM_ZLIB)
  file(GLOB ZLIB_SRC_LIST "${CMAKE_CURRENT_SOURCE_DIR}/utils/zlib/*.c")

  foreach( name ${ZLIB_SRC_LIST} )
    list( APPEND UTILS_SRC_LIST ${name} )
  endforeach()
else(NO_USE_SYSTEM_ZLIB)
  link_libraries(${ZLIB_LIBRARY})
endif(NO_USE_SYSTEM_ZLIB)

file(GLOB GLDM_SRC_LIST "${CMAKE_CURRENT_SOURCE_DIR}/utils/aesGladman/*.cpp")
foreach( name ${GLDM_SRC_LIST} )
  list( APPEND UTILS_SRC_LIST ${name} )
endforeach()

file(GLOB BZIP2_SRC_LIST "${CMAKE_CURRENT_SOURCE_DIR}/utils/bzip2/*.c")
foreach( name ${BZIP2_SRC_LIST} )
  list( APPEND UTILS_SRC_LIST ${name} )
endforeach()

file(GLOB LZMA_SRC_LIST "${CMAKE_CURRENT_SOURCE_DIR}/utils/lzma/*.c")
foreach( name ${LZMA_SRC_LIST} )
  list( APPEND UTILS_SRC_LIST ${name} )
endforeach()

file(GLOB EVENTS_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/events/*.*")
file(GLOB CORE_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/core/*.*")
file(GLOB GUI_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/gui/*.*")
file(GLOB GAME_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/game/*.*")
file(GLOB VFS_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/vfs/*.*")
file(GLOB GFX_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/gfx/*.*")
file(GLOB SOUND_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/sound/*.*")
file(GLOB SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/*.*")
file(GLOB BUILDING_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/building/*.*")
file(GLOB WALKER_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/walker/*.*")
file(GLOB WORLD_SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/world/*.*")
file(GLOB GAME_MODELS_LIST "${GAME_CONFIG_DIR}/game/*.model")
file(GLOB GUI_MODELS_LIST "${GAME_CONFIG_DIR}/gui/*.gui")

add_executable(${PROJECT_NAME} ${SRC_LIST} ${INC_LIST} ${UTILS_SRC_LIST}
               ${GAME_MODELS_LIST} ${GUI_MODELS_LIST} ${EVENTS_SOURCES_LIST}
               ${CORE_SOURCES_LIST} ${GUI_SOURCES_LIST} ${WALKER_SOURCES_LIST}
               ${BUILDING_SOURCES_LIST} ${GAME_SOURCES_LIST} ${VFS_SOURCES_LIST}
               ${GFX_SOURCES_LIST} ${SOURCES_LIST} ${SOUND_SOURCES_LIST} ${WORLD_SOURCES_LIST} )

if( BUILD_LIBINTL )
  target_link_libraries(${PROJECT_NAME} libintl)
endif( BUILD_LIBINTL )

if(WIN32)
  set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME "caesaria")
endif(WIN32)

if(UNIX)
  set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME "caesaria.linux")
endif(UNIX)

if(MACOSX)
  set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME "caesaria.macos")
endif(MACOSX)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -g")

# set compiler options
if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wno-unused-value")
endif(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")

# Copy DLL to build output directory
if(WIN32)
  if(MSVC)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} /Wall")
    include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/dependecies/msvc-include/" )
  endif(MSVC)

  set(_LIBS_FOR_DLL_SOURCE
    ${SDL_LIBRARY}
    ${SDL_MIXER_LIBRARY}
    ${SDL_TTF_LIBRARY}    
    ${PNG_LIBRARY}
  )

  if(NO_USE_SYSTEM_ZLIB)
    #
  else(NO_USE_SYSTEM_ZLIB)
    list( APPEND _LIBS_FOR_DLL_SOURCE ${ZLIB_LIBRARY} )
  endif(NO_USE_SYSTEM_ZLIB)

  foreach(_lib ${_LIBS_FOR_DLL_SOURCE})
    GET_FILENAME_COMPONENT(_lib_abs ${_lib} ABSOLUTE)
    GET_FILENAME_COMPONENT(_lib_path ${_lib} PATH)

    # Gather list of all .xml and .conf files in "/config"
    file(GLOB DLLs ${_lib_path}/*.dll
      ${_lib_path}/../bin/*.dll)

    foreach(dll ${DLLs})
      #MESSAGE (DLL: ${dll})
      add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${dll} $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    endforeach()
  endforeach(_lib)

  # Set resource path in Visual Studio as debuging argument
  if (MSVC)
    set(DEBUG_ARGS "-R \"${CMAKE_CURRENT_SOURCE_DIR}/resources\"")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/vs/opencaesar3.vcxproj.user" "${CMAKE_CURRENT_BINARY_DIR}/opencaesar3.vcxproj.user" @ONLY)
  endif(MSVC)

endif(WIN32)
