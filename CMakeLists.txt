project(opencaesar3)

cmake_minimum_required(VERSION 2.8)

aux_source_directory(. SRC_LIST)

find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(Boost COMPONENTS filesystem system REQUIRED)
find_package(OpenGL REQUIRED)
find_package(LibArchive REQUIRED)

include_directories(
    ${SDL_INCLUDE_DIR}
    ${SDL_IMAGE_INCLUDE_DIR}
    ${SDL_MIXER_INCLUDE_DIR}
    ${SDL_TTF_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${LibArchive_INCLUDE_DIR}
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
)

add_executable(${PROJECT_NAME} ${SRC_LIST})
