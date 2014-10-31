project(curl-static)

set(CURL_SOURCES_LIST
    base64.c
    connect.c
    content_encoding.c
    cookie.c
    curl_addrinfo.c
    curl_fnmatch.c
    curl_memrchr.c
    curl_rand.c
    curl_rtmp.c
    curl_sspi.c
    curl_threads.c
    dict.c
    easy.c
    escape.c
    file.c
    fileinfo.c
    formdata.c
    ftp.c
    ftplistparser.c
    getenv.c
    getinfo.c
    gtls.c
    hash.c
    hmac.c
     hostares.c
     hostasyn.c
     hostip.c
     hostip4.c
     hostip6.c
     hostsyn.c
     hostthre.c
     http.c
     http_chunks.c
     http_digest.c
     http_negotiate.c
     http_ntlm.c
     if2ip.c
     imap.c
     inet_ntop.c
     inet_pton.c
     krb4.c
     krb5.c
     ldap.c
     llist.c
     md5.c
     memdebug.c
     mprintf.c
     multi.c
     netrc.c
     nonblock.c
     nss.c
     nwlib.c
     nwos.c
     openldap.c
     parsedate.c
     pingpong.c
     polarssl.c
     pop3.c
     progress.c
     qssl.c
     rawstr.c
     rtsp.c
     security.c
     select.c
     sendf.c
     share.c
     slist.c
     smtp.c
     socks.c
     socks_gssapi.c
     socks_sspi.c
     speedcheck.c
     splay.c
     ssh.c
     sslgen.c
     ssluse.c
     strdup.c
     strequal.c
     strerror.c
     strtok.c
     strtoofft.c
     telnet.c
     tftp.c
     timeval.c
     transfer.c
     url.c
     version.c
     warnless.c
     wildcard.c
    setup_once.h
)

set(OPERATING_SYSTEM "${CMAKE_SYSTEM_NAME}")
add_definitions("-DOS=\"${CMAKE_SYSTEM_NAME}\"")

# Include all the necessary files for macros
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)

# Check if header file exists and add it to the list.
macro(CHECK_INCLUDE_FILE_CONCAT FILE VARIABLE)
  check_include_files("${CURL_INCLUDES};${FILE}" ${VARIABLE})
  if(${VARIABLE})
    add_definitions("-D${VARIABLE}")
  endif(${VARIABLE})
endmacro(CHECK_INCLUDE_FILE_CONCAT)

if(NOT UNIX)
  check_include_file_concat("ws2tcpip.h"     HAVE_WS2TCPIP_H)
  check_include_file_concat("winsock2.h"     HAVE_WINSOCK2_H)
endif(NOT UNIX)

if(NOT UNIX)
  check_include_file_concat("windows.h"      HAVE_WINDOWS_H)
  check_include_file_concat("winsock.h"      HAVE_WINSOCK_H)
endif(NOT UNIX)

check_include_file_concat("stdio.h"          HAVE_STDIO_H)
check_include_file_concat("inttypes.h"       HAVE_INTTYPES_H)
check_include_file_concat("sys/filio.h"      HAVE_SYS_FILIO_H)
check_include_file_concat("sys/ioctl.h"      HAVE_SYS_IOCTL_H)
check_include_file_concat("sys/param.h"      HAVE_SYS_PARAM_H)
check_include_file_concat("sys/poll.h"       HAVE_SYS_POLL_H)
check_include_file_concat("sys/resource.h"   HAVE_SYS_RESOURCE_H)
check_include_file_concat("sys/select.h"     HAVE_SYS_SELECT_H)
check_include_file_concat("sys/socket.h"     HAVE_SYS_SOCKET_H)
check_include_file_concat("sys/socket.h"     HAVE_SOCKET)
check_include_file_concat("sys/sockio.h"     HAVE_SYS_SOCKIO_H)
check_include_file_concat("sys/stat.h"       HAVE_SYS_STAT_H)
check_include_file_concat("sys/time.h"       HAVE_SYS_TIME_H)
check_include_file_concat("sys/types.h"      HAVE_SYS_TYPES_H)
check_include_file_concat("sys/uio.h"        HAVE_SYS_UIO_H)
check_include_file_concat("sys/un.h"         HAVE_SYS_UN_H)
check_include_file_concat("sys/utime.h"      HAVE_SYS_UTIME_H)
check_include_file_concat("alloca.h"         HAVE_ALLOCA_H)
check_include_file_concat("arpa/inet.h"      HAVE_ARPA_INET_H)
check_include_file_concat("arpa/tftp.h"      HAVE_ARPA_TFTP_H)
check_include_file_concat("assert.h"         HAVE_ASSERT_H)
check_include_file_concat("crypto.h"         HAVE_CRYPTO_H)
check_include_file_concat("des.h"            HAVE_DES_H)
check_include_file_concat("err.h"            HAVE_ERR_H)
check_include_file_concat("errno.h"          HAVE_ERRNO_H)
check_include_file_concat("fcntl.h"          HAVE_FCNTL_H)
check_include_file_concat("gssapi/gssapi.h"  HAVE_GSSAPI_GSSAPI_H)
check_include_file_concat("gssapi/gssapi_generic.h" HAVE_GSSAPI_GSSAPI_GENERIC_H)
check_include_file_concat("gssapi/gssapi_krb5.h" HAVE_GSSAPI_GSSAPI_KRB5_H)
check_include_file_concat("idn-free.h"       HAVE_IDN_FREE_H)
check_include_file_concat("ifaddrs.h"        HAVE_IFADDRS_H)
check_include_file_concat("io.h"             HAVE_IO_H)
check_include_file_concat("krb.h"            HAVE_KRB_H)
check_include_file_concat("libgen.h"         HAVE_LIBGEN_H)
check_include_file_concat("libssh2.h"        HAVE_LIBSSH2_H)
check_include_file_concat("limits.h"         HAVE_LIMITS_H)
check_include_file_concat("locale.h"         HAVE_LOCALE_H)
check_include_file_concat("net/if.h"         HAVE_NET_IF_H)
check_include_file_concat("netdb.h"          HAVE_NETDB_H)
check_include_file_concat("netinet/in.h"     HAVE_NETINET_IN_H)
check_include_file_concat("netinet/tcp.h"    HAVE_NETINET_TCP_H)
check_include_file_concat("pem.h"            HAVE_PEM_H)
check_include_file_concat("poll.h"           HAVE_POLL_H)
check_include_file_concat("pwd.h"            HAVE_PWD_H)
check_include_file_concat("rsa.h"            HAVE_RSA_H)
check_include_file_concat("setjmp.h"         HAVE_SETJMP_H)
check_include_file_concat("sgtty.h"          HAVE_SGTTY_H)
check_include_file_concat("signal.h"         HAVE_SIGNAL_H)
check_include_file_concat("ssl.h"            HAVE_SSL_H)
check_include_file_concat("stdbool.h"        HAVE_STDBOOL_H)
check_include_file_concat("stdbool.h"        HAVE_BOOL_T)
check_include_file_concat("stdint.h"         HAVE_STDINT_H)
check_include_file_concat("stdio.h"          HAVE_STDIO_H)
check_include_file_concat("stdlib.h"         HAVE_STDLIB_H)
check_include_file_concat("string.h"         HAVE_STRING_H)
check_include_file_concat("strings.h"        HAVE_STRINGS_H)
check_include_file_concat("stropts.h"        HAVE_STROPTS_H)
check_include_file_concat("termio.h"         HAVE_TERMIO_H)
check_include_file_concat("termios.h"        HAVE_TERMIOS_H)
check_include_file_concat("time.h"           HAVE_TIME_H)
check_include_file_concat("tld.h"            HAVE_TLD_H)
check_include_file_concat("unistd.h"         HAVE_UNISTD_H)
check_include_file_concat("utime.h"          HAVE_UTIME_H)
check_include_file_concat("x509.h"           HAVE_X509_H)
check_include_file_concat("process.h"        HAVE_PROCESS_H)
check_include_file_concat("stddef.h"         HAVE_STDDEF_H)
check_include_file_concat("dlfcn.h"          HAVE_DLFCN_H)
check_include_file_concat("malloc.h"         HAVE_MALLOC_H)
check_include_file_concat("memory.h"         HAVE_MEMORY_H)
check_include_file_concat("ldap.h"           HAVE_LDAP_H)
check_include_file_concat("netinet/if_ether.h" HAVE_NETINET_IF_ETHER_H)
check_include_file_concat("stdint.h"        HAVE_STDINT_H)
check_include_file_concat("sockio.h"        HAVE_SOCKIO_H)
check_include_file_concat("sys/utsname.h"   HAVE_SYS_UTSNAME_H)
check_include_file_concat("idna.h"          HAVE_IDNA_H)
check_include_file_concat("poll.h"          HAVE_POLL_FINE)

if(HAVE_TIME_H)
  add_definitions(-DHAVE_STRUCT_TIMEVAL)
endif()

if(HAVE_SYS_SOCKET_H)
  add_definitions(-DHAVE_RECV -DHAVE_SEND
                  -DRECV_TYPE_ARG1=int -DRECV_TYPE_ARG2=void* -DRECV_TYPE_ARG3=size_t
                  -DRECV_TYPE_ARG4=int -DRECV_TYPE_RETV
                  -DSEND_TYPE_ARG1=int -DSEND_TYPE_ARG2=void*
                  -DSEND_TYPE_ARG3=size_t -DSEND_TYPE_ARG4=int
                  -DSEND_TYPE_RETV -DSEND_QUAL_ARG2 -DHAVE_FCNTL_O_NONBLOCK )
endif()

SET(CMAKE_EXTRA_INCLUDE_FILES "include/curl/curl.h")

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  MESSAGE( "64 bits compiler detected" )
  SET( EX_PLATFORM 64 )
  SET( EX_PLATFORM_NAME "x64" )
else( CMAKE_SIZEOF_VOID_P EQUAL 8 ) 
  MESSAGE( "32 bits compiler detected" )
  SET( EX_PLATFORM 32 )
  SET( EX_PLATFORM_NAME "x86" )
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

if( EX_PLATFORM EQUAL 64 )
  add_definitions( "-DSIZEOF_LONG=8" )
  add_definitions( "-DSIZEOF_SHORT=2" )
  add_definitions( "-DSIZEOF_INT=8" )
else(EX_PLATFORM EQUAL 64 )
  add_definitions( "-DSIZEOF_LONG=8" )
  add_definitions( "-DSIZEOF_SHORT=2" )
  add_definitions( "-DSIZEOF_INT=4" ) 
endif( EX_PLATFORM EQUAL 64 )

# Different sizeofs, etc.

#    define CURL_SIZEOF_LONG        4
#    define CURL_TYPEOF_CURL_OFF_T  long long
#    define CURL_FORMAT_CURL_OFF_T  "lld"
#    define CURL_FORMAT_CURL_OFF_TU "llu"
#    define CURL_FORMAT_OFF_T       "%lld"
#    define CURL_SIZEOF_CURL_OFF_T  8
#    define CURL_SUFFIX_CURL_OFF_T  LL
#    define CURL_SUFFIX_CURL_OFF_TU ULL

set(CURL_SIZEOF_LONG ${SIZEOF_LONG})

if(SIZEOF_LONG EQUAL 8)
  set(CURL_TYPEOF_CURL_OFF_T long)
  set(CURL_SIZEOF_CURL_OFF_T 8)
  set(CURL_FORMAT_CURL_OFF_T "ld")
  set(CURL_FORMAT_CURL_OFF_TU "lu")
  set(CURL_FORMAT_OFF_T "%ld")
  set(CURL_SUFFIX_CURL_OFF_T L)
  set(CURL_SUFFIX_CURL_OFF_TU UL)
endif(SIZEOF_LONG EQUAL 8)

if(SIZEOF_LONG_LONG EQUAL 8)
  set(CURL_TYPEOF_CURL_OFF_T "long long")
  set(CURL_SIZEOF_CURL_OFF_T 8)
  set(CURL_FORMAT_CURL_OFF_T "lld")
  set(CURL_FORMAT_CURL_OFF_TU "llu")
  set(CURL_FORMAT_OFF_T "%lld")
  set(CURL_SUFFIX_CURL_OFF_T LL)
  set(CURL_SUFFIX_CURL_OFF_TU ULL)
endif(SIZEOF_LONG_LONG EQUAL 8)

if(NOT CURL_TYPEOF_CURL_OFF_T)
  set(CURL_TYPEOF_CURL_OFF_T ${ssize_t})
  set(CURL_SIZEOF_CURL_OFF_T ${SIZEOF_SSIZE_T})
  # TODO: need adjustment here.
  set(CURL_FORMAT_CURL_OFF_T "ld")
  set(CURL_FORMAT_CURL_OFF_TU "lu")
  set(CURL_FORMAT_OFF_T "%ld")
  set(CURL_SUFFIX_CURL_OFF_T L)
  set(CURL_SUFFIX_CURL_OFF_TU LU)
endif(NOT CURL_TYPEOF_CURL_OFF_T)

if(HAVE_SIZEOF_LONG_LONG)
  set(HAVE_LONGLONG 1)
  set(HAVE_LL 1)
endif(HAVE_SIZEOF_LONG_LONG)

SET(CMAKE_EXTRA_INCLUDE_FILES)

include_directories( ${DEP_SOURCE_DIR} )

add_definitions( -DCURL_STATICLIB )
add_definitions( -DCURL_DISABLE_LDAP )

add_library(${PROJECT_NAME} STATIC ${CURL_SOURCES_LIST})


# Expose public includes to other through cache variable.
set(CURL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

set(CURL_LIBRARY ${PROJECT_NAME}
    CACHE INTERNAL "${PROJECT_NAME}: Library Name" FORCE)
