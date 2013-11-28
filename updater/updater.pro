TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
OBJECTS_DIR = build
TARGET = updater.linux

win32: {
LIBS += -lpsapi -lws2_32
LIBS += -L../../dependencies/curl/lib -lcurl
}

unix: {
LIBS += -lpthread -lz -lcurl
}

INCLUDEPATH += ../source
INCLUDEPATH += ../utils
INCLUDEPATH += ../dependencies/curl/include

SOURCES += main.cpp \
    ../source/core/thread.cpp \
    ../source/core/logger.cpp \
    ../source/core/mutex.cpp \
    ../source/core/threadevent.cpp \
    ../source/core/threadtask.cpp \
    ../source/core/stringhelper.cpp \
    ../source/core/time.cpp \
    ../source/core/variant.cpp \
    ../source/vfs/filesystem.cpp \
    ../source/core/bytearray.cpp \
    ../source/vfs/file.cpp \
    ../source/vfs/archive_zip.cpp \
    ../utils/lzma/LzmaDec.c \
    ../source/vfs/memfile.cpp \
    ../source/vfs/filenative_impl.cpp \
    ../utils/bzip2/randtable.c \
    ../utils/bzip2/huffman.c \
    ../utils/bzip2/decompress.c \
    ../utils/bzip2/crctable.c \
    ../utils/bzip2/bzlib.c \
    ../utils/bzip2/bzcompress.c \
    ../utils/bzip2/blocksort.c \
    ../utils/aesGladman/sha2.cpp \
    ../utils/aesGladman/sha1.cpp \
    ../utils/aesGladman/pwd2key.cpp \
    ../utils/aesGladman/hmac.cpp \
    ../utils/aesGladman/fileenc.cpp \
    ../utils/aesGladman/aestab.cpp \
    ../utils/aesGladman/aeskey.cpp \
    ../utils/aesGladman/aescrypt.cpp \
    ../utils/zlib/zutil.c \
    ../utils/zlib/uncompr.c \
    ../utils/zlib/trees.c \
    ../utils/zlib/inftrees.c \
    ../utils/zlib/inflate.c \
    ../utils/zlib/inffast.c \
    ../utils/zlib/infback.c \
    ../utils/zlib/gzwrite.c \
    ../utils/zlib/gzread.c \
    ../utils/zlib/gzlib.c \
    ../utils/zlib/gzclose.c \
    ../utils/zlib/deflate.c \
    ../utils/zlib/crc32.c \
    ../utils/zlib/compress.c \
    ../utils/zlib/adler32.c \
    ../source/vfs/path.cpp \
    ../source/vfs/entries.cpp \
    ../source/vfs/directory.cpp \
    util.cpp \
    consoleupdater.cpp \
    updater/updater.cpp \
    updater/updatecontroller.cpp \
    http/httpconnection.cpp \
    inifile.cpp \
    http/httprequest.cpp \
    http/mirrordownload.cpp \
    http/downloadmanager.cpp \
    http/download.cpp \
    packager/packager.cpp



HEADERS += \
    ../source/core/thread.hpp \
    ../source/core/logger.hpp \
    ../source/core/mutex.hpp \
    ../source/core/threadevent.hpp \
    ../source/core/threadtask.hpp \
    ../source/core/stringhelper.hpp \
    ../source/core/time.hpp \
    ../source/core/variant.hpp \
    ../source/vfs/filesystem.hpp \
    ../source/vfs/filelist.hpp \
    ../source/core/bytearray.hpp \
    ../source/vfs/file.hpp \
    ../source/vfs/archive_zip.hpp \
    ../utils/lzma/Types.h \
    ../utils/lzma/LzmaDec.h \
    ../source/vfs/memfile.hpp \
    ../source/vfs/filenative_impl.hpp \
    ../source/vfs/entity.hpp \
    ../source/vfs/archive.hpp \
    ../utils/bzip2/bzlib_private.h \
    ../utils/bzip2/bzlib.h \
    ../utils/aesGladman/sha2.h \
    ../utils/aesGladman/sha1.h \
    ../utils/aesGladman/pwd2key.h \
    ../utils/aesGladman/hmac.h \
    ../utils/aesGladman/fileenc.h \
    ../utils/aesGladman/aesopt.h \
    ../utils/aesGladman/aes.h \
    ../utils/zlib/zutil.h \
    ../utils/zlib/zlib.h \
    ../utils/zlib/zconf.h \
    ../utils/zlib/trees.h \
    ../utils/zlib/inftrees.h \
    ../utils/zlib/inflate.h \
    ../utils/zlib/inffixed.h \
    ../utils/zlib/inffast.h \
    ../utils/zlib/gzguts.h \
    ../utils/zlib/deflate.h \
    ../utils/zlib/crc32.h \
    minizip/ioapi.h \
    ../source/vfs/entries.hpp \
    ../source/vfs/directory.hpp \
    util.hpp \
    constants.hpp \
    consoleupdater.hpp \
    updater/updateview.hpp \
    updater/updater.hpp \
    updater/updatecontroller.hpp \
    updater/updateroptions.hpp \
    updater/progresshandler.hpp \
    programoptions.hpp \
    http/httpconnection.hpp \
    packager/packager.hpp \
    exceptionsafethread.hpp \
    http/downloadmanager.hpp \
    http/download.hpp \
    http/httprequest.hpp \
    http/mirrordownload.hpp \
    http/mirrorlist.hpp \
    inifile.hpp \
    packager/packageroptions.hpp \
    version.hpp \
    releasefileset.hpp
