project(curl-static)

set(CURL_SOURCES_LIST
    amigaos.c
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
)

include_directories( ${DEP_SOURCE_DIR} )

#set(CURLLIB_NAME_STATIC "curl")

add_definitions( -DCURL_STATICLIB )
add_definitions( -DCURL_DISABLE_LDAP )

add_library(${CURLLIB_NAME} STATIC ${CURL_SOURCES_LIST})
