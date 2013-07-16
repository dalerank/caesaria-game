cmake_minimum_required(VERSION 2.8)
project(sgreader)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Modules")

set(HEADERS 
           aboutdialog.h
           imagetreeitem.h
           licensedialog.h
           mainwindow.h
           sgbitmap.h
           sgfile.h
           sgimage.h
           gui/filelistpage.h
           gui/inputdirpage.h
           gui/outputdirpage.h
           gui/progresspage.h
           gui/extractthread.h
           gui/extractwizard.h
)

set(SOURCES 
           aboutdialog.cpp
           imagetreeitem.cpp
           licensedialog.cpp
           main.cpp
           mainwindow.cpp
           sgbitmap.cpp
           sgfile.cpp
           sgimage.cpp
           gui/filelistpage.cpp
           gui/inputdirpage.cpp
           gui/outputdirpage.cpp
           gui/progresspage.cpp
           gui/extractthread.cpp
           gui/extractwizard.cpp
)

set(RESOURCES sgreader.qrc)

# QT4
set(CMAKE_AUTOMOC TRUE)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(Qt COMPONENTS Gui REQUIRED)
INCLUDE(${QT_USE_FILE})
link_libraries(${QT_LIBRARIES})
ADD_DEFINITIONS(${QT_DEFINITIONS})

#QT4_WRAP_CPP(HEADERS_MOC ${HEADERS} main.cpp)
QT4_ADD_RESOURCES(RESOURCES_RCC ${RESOURCES})

# Qxt
find_package(Qxt REQUIRED)
include_directories(${QXT_CORE_INCLUDE_PATH})
link_libraries(${QXT_CORE_LIB})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${RESOURCES_RCC})

