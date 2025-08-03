/*
 * Copyright (c) Branko Premzel.
 *
 * SPDX-License-Identifier: MIT
 */

/*******************************************************************************
 * @file    platform_compat.h
 * @brief   Platform compatibility header for cross-platform builds
 ******************************************************************************/

#ifndef PLATFORM_COMPAT_H
#define PLATFORM_COMPAT_H

// Handle __declspec for non-Windows platforms
#ifndef _WIN32
    #define __declspec(x)
    #define _MAX_PATH PATH_MAX
    #define CP_UTF8 65001
#endif

#ifdef _WIN32
    // Windows includes
    #include <direct.h>
    #include <windows.h>
    #include <io.h>
    
    // Windows specific functions
    #define PATH_SEPARATOR '\\'
    #define chdir_compat _chdir
    #define wchdir_compat _wchdir
    #define getcwd_compat _getcwd
    #define wgetcwd_compat _wgetcwd
    #define fseeki64_compat _fseeki64
    #define ftelli64_compat _ftelli64
    
#else
    // Linux/Unix includes
    #include <unistd.h>
    #include <sys/types.h>
    #include <limits.h>
    #include <errno.h>
    #include <sys/stat.h>
    
    // Linux/Unix compatibility
    #define PATH_SEPARATOR '/'
    #define chdir_compat chdir
    #define getcwd_compat getcwd
    #define fseeki64_compat fseeko
    #define ftelli64_compat ftello
    #define _stat stat
    #define _set_errno(x) errno = (x)
    #define strcpy_s(dest, size, src) strcpy(dest, src)
    #define SetConsoleOutputCP(x)
    #define SetConsoleCP(x)
    #define wprintf_s wprintf
    #define _wcserror(x) strerror(x)
    #define _fcloseall() fflush(NULL)
    #define _wchdir(x) wchdir_compat(x)
    #define _wgetcwd(x, y) wgetcwd_compat(x, y)
    #define _chdir(x) chdir(x)
    #define _get_pgmptr(x) get_pgmptr_compat(x)
    #define _fullpath(dst, src, size) realpath(src, dst)
    #define _wfullpath(dst, src, size) wfullpath_compat(dst, src, size)
    #define MultiByteToWideChar(cp, flags, src, srclen, dst, dstlen) MultiByteToWideChar_compat(cp, flags, src, srclen, dst, dstlen)
    
    // Exception handling compatibility
    #define __try if (1)
    #define __except(x) if (0)
    #define __finally if (1)
    #define EXCEPTION_EXECUTE_HANDLER 1
    
    // Sleep function
    #define Sleep(x) usleep((x) * 1000)
    
    // Integer types
    #define __int64 int64_t
    typedef int errno_t;
    
    // File functions
    #define fopen_s(pFile, filename, mode) ((*(pFile) = fopen((filename), (mode))) ? 0 : errno)
    
    // Wide character support for Linux
    #include <wchar.h>
    #include <locale.h>
    
    // Define wide character directory functions for Linux
    int wchdir_compat(const wchar_t *path);
    wchar_t* wgetcwd_compat(wchar_t *buffer, size_t size);
    int get_pgmptr_compat(char **ptr);
    wchar_t* wfullpath_compat(wchar_t *dst, const wchar_t *src, size_t size);
    int MultiByteToWideChar_compat(unsigned int cp, unsigned long flags, const char *src, int srclen, wchar_t *dst, int dstlen);
    FILE* _wfopen(const wchar_t *filename, const wchar_t *mode);
    int _wremove(const wchar_t *filename);
    int _wrename(const wchar_t *oldname, const wchar_t *newname);
    
    // Define MAX_PATH if not available
    #ifndef MAX_PATH
        #ifdef PATH_MAX
            #define MAX_PATH PATH_MAX
        #else
            #define MAX_PATH 4096
        #endif
    #endif
    
    // Define int64_t if needed
    #include <stdint.h>
    
#endif

// Common includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <locale.h>
#include <stdint.h>
#include <errno.h>

#endif // PLATFORM_COMPAT_H