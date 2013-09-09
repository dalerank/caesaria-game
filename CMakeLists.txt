project(opencaesar3)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/" ${CMAKE_MODULE_PATH})
#set(NO_USE_SYSTEM_ZLIB ON)

aux_source_directory(. SRC_LIST)
file(GLOB INC_LIST "*.hpp")

if(OPENCAESAR3_CROSSCOMPILING)
  MESSAGE("Prepare wintoolchain for cross compilation")
  set(DEP_URL "https://www.dropbox.com/s/4ozo2q3yewga64n/dependencies-mingw.7z" )
  set(DEP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies")
  set(DEP_DOWNLOAD_PATH "${DEP_DIR}/dependencies-mingw.7z")

  if(NOT EXISTS "${DEP_DOWNLOAD_PATH}")
    MESSAGE("Can't find ${DEP_DOWNLOAD_PATH}")
    MESSAGE("Try download dependencies-mingw.7z from ${DEP_URL}")
    file( DOWNLOAD ${DEP_URL} ${DEP_DOWNLOAD_PATH} )
  endif()

  execute_process( COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/cmake/xcompile/getdeps.sh )
endif(OPENCAESAR3_CROSSCOMPILING)

find_package(SDL REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Libintl REQUIRED)
find_package(PNG REQUIRED)
find_package(zlib REQUIRED)

include_directories(
  ${SDL_INCLUDE_DIR}
  ${SDL_MIXER_INCLUDE_DIR}
  ${SDL_TTF_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${LIBINTL_INCLUDE_DIR}
  ${PNG_INCLUDE_DIR}
)

link_libraries(
  ${SDL_LIBRARY}
  ${SDL_MIXER_LIBRARY}
  ${SDL_TTF_LIBRARY}
  ${OPENGL_gl_LIBRARY}
  ${LIBINTL_LIBRARIES}
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

file(GLOB DATA_MODELS_LIST "${CMAKE_CURRENT_SOURCE_DIR}/models/*.model")

add_executable(${PROJECT_NAME} ${SRC_LIST} ${INC_LIST} ${UTILS_SRC_LIST} ${DATA_MODELS_LIST})

set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME "caesar3")

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
    ${LIBINTL_LIBRARIES}
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
