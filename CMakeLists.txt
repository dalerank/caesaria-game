project(opencaesar3)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/" ${CMAKE_MODULE_PATH})

aux_source_directory(. SRC_LIST)

find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(OpenGL REQUIRED)
find_package(LibArchive REQUIRED)
find_package(Libintl REQUIRED)

set(Boost_USE_STATIC_LIBS   ON)
find_package(Boost COMPONENTS filesystem system REQUIRED)

include_directories(
    ${SDL_INCLUDE_DIR}
    ${SDL_IMAGE_INCLUDE_DIR}
    ${SDL_MIXER_INCLUDE_DIR}
    ${SDL_TTF_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${LibArchive_INCLUDE_DIR}
	${LIBINTL_INCLUDE_DIR}
)

link_libraries(
    ${SDL_LIBRARY}
    ${SDL_IMAGE_LIBRARY}
    ${SDL_MIXER_LIBRARY}
    ${SDL_TTF_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${OPENGL_gl_LIBRARY}
    ${LibArchive_LIBRARY}
	${LIBINTL_LIBRARIES}
)

add_definitions(-DBOOST_ALL_NO_LIB)

# Copy DLL to build output directory
add_executable(${PROJECT_NAME} ${SRC_LIST})

if(WIN32)
	set(_LIBS_FOR_DLL_SOURCE
			${SDL_LIBRARY}
			${SDL_IMAGE_LIBRARY}
			${SDL_MIXER_LIBRARY}
			${SDL_TTF_LIBRARY}
			${LibArchive_LIBRARY}
			${LIBINTL_LIBRARIES}
		)

	foreach(_lib ${_LIBS_FOR_DLL_SOURCE})
		GET_FILENAME_COMPONENT(_lib_abs ${_lib} ABSOLUTE)
		GET_FILENAME_COMPONENT(_lib_path ${_lib} PATH)

		# Gather list of all .xml and .conf files in "/config"
		file(GLOB DLLs 	${_lib_path}/*.dll
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