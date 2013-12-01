/*-
 * Copyright (c) 2003-2006 Tim Kientzle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in this position and unchanged.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#ifndef LIBARCHIVE_NONPOSIX_H_INCLUDED
#define LIBARCHIVE_NONPOSIX_H_INCLUDED

/* Start of configuration for native Win32  */
/* TODO: Fix this. */

#include <sys/types.h>
#include <errno.h>
#define set_errno(val)	((errno)=val)
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
//#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
//#include <sys/time.h>
#include <process.h>

/* Basic definitions for system and integer types. */
#undef uid_t
#undef gid_t
#undef id_t
#undef ssize_t
typedef int uid_t;
typedef int gid_t;

typedef int id_t;
typedef int mode_t;
typedef int pid_t;

#ifndef _SSIZE_T_
typedef long ssize_t;
#define _SSIZE_T_
#endif /* _SSIZE_T_ */

/* Replacement for major/minor/makedev. */
#define	major(x) ((int)(0x00ff & ((x) >> 8)))
#define	minor(x) ((int)(0xffff00ff & (x)))
#define	makedev(maj,min) ((0xff00 & ((maj)<<8))|(0xffff00ff & (min)))

#define	EFTYPE 7
#ifndef STDERR_FILENO
#define	STDERR_FILENO 2
#endif /* STDERR_FILENO  */

/* Alias the Windows _function to the POSIX equivalent. */
#define	chdir		_chdir
#define	chmod		_chmod
#define	close		_close
#define	dup			_dup
#define	dup2			_dup2
#define	execlp		_execlp
#define	fstat		_fstat
#define	lseek		_lseek
#define	lstat		_stat
#define	open			_open
#define	stat			_stat
#define	mkdir(d,m)	_mkdir(d)
#define	mktemp		_mktemp
#define	read			_read
#define	rmdir		_rmdir
#define	strdup		_strdup
#define	stricmp		_stricmp
#define	tzset		_tzset
#define	umask		_umask
#define	write		_write

#ifdef _S_IFBLK
# undef _S_IFBLK
#endif
#define  _S_IFBLK        0x6000    
#define  _S_IFLNK        0xA000    /* symbolic link */
#define  _S_IFSOCK       0xC000    /* socket */
#ifdef	_S_IFMT
 #undef	_S_IFMT
#endif /* _S_IFMT */
#define  _S_IFMT		0xF000

#define  _IFLNK          _S_IFLNK
#define  _IFSOCK         _S_IFSOCK
#define  _S_ISLNK(m)     (((m) & _S_IFMT) == _S_IFLNK)
#define  _S_ISSOCK(m)    (((m) & _S_IFMT) == _S_IFSOCK)

#define __S_IFMT         _S_IFMT
#define __S_IFDIR        _S_IFDIR
#define __S_IFCHR        _S_IFCHR
#define __S_IFBLK        _S_IFBLK
#define __S_IFREG        _S_IFREG
#define __S_IFIFO        _S_IFIFO
#define __S_IFLNK        _S_IFLNK
#define __S_IFSOCK       _S_IFSOCK

#ifdef	_IFMT
 #undef	_IFMT
#endif /* _IFMT */
#define  _IFMT           _S_IFMT   /* File type mask */
#define  _S_ISUID        0004000   /* set user id on execution */
#define  _S_ISGID        0002000   /* set group id on execution */
#define  _S_ISVTX        0001000   /* save swapped text even after use */
#define  _S_IRWXG        (_S_IRWXU >> 3)
#define  _S_IXGRP        (_S_IXUSR >> 3) /* read permission, group */
#define  _S_IWGRP        (_S_IWUSR >> 3) /* write permission, grougroup */
#define  _S_IRGRP        (_S_IRUSR >> 3) /* execute/search permission, group */
#define  _S_IRWXO        (_S_IRWXG >> 3) 
#define  _S_IXOTH        (_S_IXGRP >> 3) /* read permission, other */
#define  _S_IWOTH        (_S_IWGRP >> 3) /* write permission, other */
#define  _S_IROTH        (_S_IRGRP  >> 3) /* execute/search permission, other */

#ifndef _NO_OLDNAMES

#ifdef	S_IFMT
 #undef	S_IFMT
#endif /* S_IFMT */
#define 	S_IFMT         _S_IFMT

#define   S_ISUID        _S_ISUID
#define   S_ISGID        _S_ISGID
#define   S_ISVTX        _S_ISVTX

#define   S_IRWXG        _S_IRWXG
#define   S_IXGRP        _S_IXGRP
#define   S_IWGRP        _S_IWGRP
#define   S_IRGRP        _S_IRGRP
#define   S_IRWXO        _S_IRWXO
#define   S_IXOTH        _S_IXOTH
#define   S_IWOTH        _S_IWOTH
#define   S_IROTH        _S_IROTH

#define   S_IFBLK        _S_IFBLK
#define   S_IFLNK        _S_IFLNK
#define   S_IFSOCK       _S_IFSOCK

#define   S_ISLNK(m)    _S_ISLNK(m)
#define   S_ISSOCK(m)   _S_ISSOCK(m)

#endif    /* Not _NO_OLDNAMES */

#define	F_DUPFD	  	0	/* Duplicate file descriptor.  */
#define	F_GETFD		1	/* Get file descriptor flags.  */
#define	F_SETFD		2	/* Set file descriptor flags.  */
#define	F_GETFL		3	/* Get file status flags.  */
#define	F_SETFL		4	/* Set file status flags.  */
#define	F_GETOWN		5	/* Get owner (receiver of SIGIO).  */
#define	F_SETOWN		6	/* Set owner (receiver of SIGIO).  */
#define	F_GETLK		7	/* Get record locking info.  */
#define	F_SETLK		8	/* Set record locking info (non-blocking).  */
#define	F_SETLKW		9	/* Set record locking info (blocking).  */

/* XXX missing */
#define F_GETLK64	7	/* Get record locking info.  */
#define F_SETLK64	8	/* Set record locking info (non-blocking).  */
#define F_SETLKW64	9	/* Set record locking info (blocking).  */

/* File descriptor flags used with F_GETFD and F_SETFD.  */
#define	FD_CLOEXEC	1	/* Close on exec.  */

#define	O_NONBLOCK	0x0004	/* Non-blocking I/O.  */
#define 	O_NDELAY		O_NONBLOCK


#ifndef HAVE_GETUID
uid_t
extern inline getuid()
{
	return 0;
}	
#define HAVE_GETUID 1
#endif /* HAVE_GETUID */

#ifndef HAVE_GETEUID
uid_t
extern inline geteuid()
{
	return 0;
}	
#define HAVE_GETEUID 1
#endif /* HAVE_GETEUID */

#ifndef HAVE_GETGID
gid_t
extern inline getgid()
{
	return 0;
}	
#define HAVE_GETGID 1
#endif /* HAVE_GETGID */

#ifndef O_BINARY
# ifdef _O_BINARY
#  define O_BINARY _O_BINARY
# else
#  define O_BINARY 0
# endif
#endif /* O_BINARY */

#if O_BINARY
# define setmode(f,m)	_setmode(f,m)
# define SET_BINARY(f)	do {if (!isatty(f)) setmode(f,O_BINARY);} while (0)
#endif /* not O_BINARY */

#ifdef _INTEGRAL_MAX_BITS 
# define _FILE_OFFSET_BITS _INTEGRAL_MAX_BITS
# if _INTEGRAL_MAX_BITS >= 64
#  define _LARGEFILE64_SOURCE
# endif /*  _INTEGRAL_MAX_BITS >= 64 */
#endif /* _INTEGRAL_MAX_BITS */

#ifdef _LARGEFILE_SOURCE
# define __USE_LARGEFILE 1		/* declare fseeko and ftello */
#endif

#ifdef _LARGEFILE64_SOURCE
# define __USE_LARGEFILE64    1	/* declare 64-bit functions */
#endif

#if defined _FILE_OFFSET_BITS && _FILE_OFFSET_BITS == 64
# define __USE_FILE_OFFSET64  1	/* replace 32-bit functions by 64-bit ones */
#endif

#if (__USE_LARGEFILE || __USE_LARGEFILE64) && __USE_FILE_OFFSET64
/* replace stat and seek by their large-file equivalents */
#undef	stat
#define   stat		_stati64
#define   _stat	_stati64
#undef	lstat
#define   lstat	_stati64
#undef	fstat
#define   fstat	_fstati64
#define   _fstat	_fstati64
#define	off_t	__int64

#undef	lseek
#define	lseek       _lseeki64
#define	lseek64     _lseeki64
#define	tell        _telli64
#define	tell64      _telli64

#ifdef __MINGW32__
# define fseek      fseeko64
# define fseeko     fseeko64
# define ftell      ftello64
# define ftello     ftello64
# define ftell64    ftello64
#endif /* __MINGW32__ */
#endif /* LARGE_FILES */

/* End of Win32 definitions. */

#ifdef __cplusplus
extern "C" {
#endif

extern char *fullname (const char *name);
extern int readlink (const char *path, char *buf, size_t len);
extern int link (const char *from, const char *to);
extern int symlink (const char *from, const char *to);
extern int utimes (const char *path, const struct timeval times[2]);
#define HAVE_UTIMES 1
extern int fcntl (int fd, int cmd, ...);
extern int fork ();
extern pid_t vfork (void);
extern int setenv (const char *name, const char *value, int replace);
extern int unsetenv (const char *name);
extern int pipe (int pipedes[2]);
extern unsigned int sleep (unsigned int seconds);
extern pid_t waitpid (pid_t pid, int *stat_loc, int options);

#ifdef __cplusplus
}
#endif

#endif /* LIBARCHIVE_NONPOSIX_H_INCLUDED  */
