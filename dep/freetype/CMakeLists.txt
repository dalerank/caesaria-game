project(freetype)

add_definitions(-DFT2_BUILD_LIBRARY)

file(GLOB CFF_SOURCES "src/cff/*.c")
file(GLOB TYPE1_SOURCES "src/type1/*.c")
file(GLOB AF_SOURCES "src/autofit/*.c")
file(GLOB CID_SOURCES "src/cid/*.c")
file(GLOB PFR_SOURCES "src/pfr/*.c")
file(GLOB T42_SOURCES "src/type42/*.c")
file(GLOB PCF_SOURCES "src/pcf/*.c")
file(GLOB PSAUX_SOURCES "src/psaux/*.c")
file(GLOB BASE_SOURCES "src/base/*.c")
file(GLOB PSHINTER_SOURCES "src/pshinter/*.c")
file(GLOB RASTER_SOURCES "src/raster/*.c")
file(GLOB SFNT_SOURCES "src/sfnt/*.c")
file(GLOB SMOOTH_SOURCES "src/smooth/*.c")
file(GLOB TTF_SOURCES "src/truetype/*.c")
file(GLOB BDF_SOURCES "src/bdf/*.c")

set( freetype_SRC
    src/winfonts/winfnt.c
    src/gzip/ftgzip.c
    src/lzw/ftlzw.c
    src/psnames/psmodule.c
)

include_directories( 
    builds/unix
    src/base 
    src/truetype 
    src/type1 
    src/cff 
    src/cid 
    src/pfr 
    src/type42 
    src/winfonts 
    src/pcf 
    src/bdf 
    src/sfnt 
    src/autofit 
    src/pshinter 
    src/raster 
    src/smooth 
    src/cache 
    src/gzip 
    src/lzw 
    src/psaux 
    src/psnames
    include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${ZLIB_HOME}
)

add_library(${FREETYPELIB_NAME} STATIC ${CFF_SOURCES} ${freetype_SRC} ${TYPE1_SOURCES}
				       ${AF_SOURCES} ${CID_SOURCES} ${PFR_SOURCES} ${T42_SOURCES}
				       ${PCF_SOURCES} ${PSAUX_SOURCES} ${BASE_SOURCES}
				       ${PSHINTER_SOURCES} ${RASTER_SOURCES} ${SMOOTH_SOURCES}
				       ${SFNT_SOURCES} ${TTF_SOURCES} ${BDF_SOURCES} )
#target_link_libraries(${FREETYPELIB_NAME} ${ZLIB_NAME})


# Expose public includes to other through cache variable.
set(FREETYPE_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

set(FREETYPE_LIBRARY ${PROJECT_NAME}
    CACHE INTERNAL "${PROJECT_NAME}: Library Name" FORCE)
