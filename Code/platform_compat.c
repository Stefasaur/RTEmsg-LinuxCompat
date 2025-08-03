/*
 * Copyright (c) Branko Premzel.
 *
 * SPDX-License-Identifier: MIT
 */

/*******************************************************************************
 * @file    platform_compat.c
 * @brief   Platform compatibility implementation for Linux/Unix systems
 ******************************************************************************/

#ifndef _WIN32

#include "platform_compat.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <locale.h>

/**
 * @brief Change directory using wide character path (Linux implementation)
 * @param path Wide character path
 * @return 0 on success, -1 on error
 */
int wchdir_compat(const wchar_t *path)
{
    if (path == NULL) {
        errno = EINVAL;
        return -1;
    }
    
    // Convert wide char to multibyte
    size_t len = wcstombs(NULL, path, 0);
    if (len == (size_t)-1) {
        return -1;
    }
    
    char *mbpath = malloc(len + 1);
    if (mbpath == NULL) {
        errno = ENOMEM;
        return -1;
    }
    
    wcstombs(mbpath, path, len + 1);
    int result = chdir(mbpath);
    free(mbpath);
    
    return result;
}

/**
 * @brief Get current working directory in wide characters (Linux implementation)
 * @param buffer Buffer to store the path (can be NULL for auto-allocation)
 * @param size Size of buffer in wide characters (can be 0 for auto-sizing)
 * @return Pointer to buffer on success, NULL on error
 */
wchar_t* wgetcwd_compat(wchar_t *buffer, size_t size)
{
    // Get current directory in multibyte
    char *mbpath = getcwd(NULL, 0);
    if (mbpath == NULL) {
        return NULL;
    }
    
    // Convert to wide char to determine required size
    size_t len = mbstowcs(NULL, mbpath, 0);
    if (len == (size_t)-1) {
        free(mbpath);
        errno = EILSEQ;
        return NULL;
    }
    
    // Auto-allocate buffer if needed (like Windows _wgetcwd)
    wchar_t *result_buffer;
    if (buffer == NULL || size == 0) {
        result_buffer = malloc((len + 1) * sizeof(wchar_t));
        if (result_buffer == NULL) {
            free(mbpath);
            errno = ENOMEM;
            return NULL;
        }
    } else {
        if (len >= size) {
            free(mbpath);
            errno = ERANGE;
            return NULL;
        }
        result_buffer = buffer;
    }
    
    // Convert multibyte to wide char
    mbstowcs(result_buffer, mbpath, len + 1);
    free(mbpath);
    
    return result_buffer;
}

/**
 * @brief Get program path (Linux implementation)
 * @param ptr Pointer to store the program path
 * @return 0 on success, non-zero on error
 */
int get_pgmptr_compat(char **ptr)
{
    static char program_path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", program_path, sizeof(program_path) - 1);
    
    if (len == -1) {
        return -1;
    }
    
    program_path[len] = '\0';
    *ptr = program_path;
    return 0;
}

/**
 * @brief Get full path in wide characters (Linux implementation)
 * @param dst Destination buffer
 * @param src Source path
 * @param size Size of destination buffer
 * @return Pointer to dst on success, NULL on error
 */
wchar_t* wfullpath_compat(wchar_t *dst, const wchar_t *src, size_t size)
{
    if (dst == NULL || src == NULL || size == 0) {
        errno = EINVAL;
        return NULL;
    }
    
    // Convert wide char to multibyte
    size_t len = wcstombs(NULL, src, 0);
    if (len == (size_t)-1) {
        return NULL;
    }
    
    char *mbsrc = malloc(len + 1);
    if (mbsrc == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    
    wcstombs(mbsrc, src, len + 1);
    
    // Get real path
    char resolved[PATH_MAX];
    if (realpath(mbsrc, resolved) == NULL) {
        free(mbsrc);
        return NULL;
    }
    
    free(mbsrc);
    
    // Convert back to wide char
    len = mbstowcs(NULL, resolved, 0);
    if (len == (size_t)-1 || len >= size) {
        errno = ERANGE;
        return NULL;
    }
    
    mbstowcs(dst, resolved, size);
    return dst;
}

/**
 * @brief Convert multibyte string to wide character string (Linux implementation)
 * @param cp Code page (ignored on Linux)
 * @param flags Conversion flags (ignored on Linux)
 * @param src Source multibyte string
 * @param srclen Length of source string (-1 for null-terminated)
 * @param dst Destination wide character buffer
 * @param dstlen Size of destination buffer in wide characters
 * @return Number of wide characters written, or 0 on error
 */
int MultiByteToWideChar_compat(unsigned int cp, unsigned long flags, const char *src, int srclen, wchar_t *dst, int dstlen)
{
    if (src == NULL || dst == NULL || dstlen <= 0) {
        return 0;
    }
    
    // Set locale for proper UTF-8 conversion
    static int locale_set = 0;
    if (!locale_set) {
        setlocale(LC_CTYPE, "C.UTF-8");
        locale_set = 1;
    }
    
    // Handle null-terminated string
    if (srclen == -1) {
        srclen = strlen(src);
    }
    
    // Create a null-terminated copy for mbstowcs
    char *temp_src = malloc(srclen + 1);
    if (temp_src == NULL) {
        return 0;
    }
    
    strncpy(temp_src, src, srclen);
    temp_src[srclen] = '\0';
    
    // Convert using mbstowcs
    size_t result = mbstowcs(dst, temp_src, dstlen - 1);
    free(temp_src);
    
    if (result == (size_t)-1) {
        return 0;  // Conversion error
    }
    
    // Ensure null termination
    if (result < (size_t)(dstlen - 1)) {
        dst[result] = L'\0';
    } else {
        dst[dstlen - 1] = L'\0';
        result = dstlen - 1;
    }
    
    return (int)result;
}

/**
 * @brief Open file with wide character filename (Linux implementation)
 * @param filename Wide character filename
 * @param mode Wide character mode
 * @return FILE pointer on success, NULL on error
 */
FILE* _wfopen(const wchar_t *filename, const wchar_t *mode)
{
    if (filename == NULL || mode == NULL) {
        errno = EINVAL;
        return NULL;
    }
    
    // Convert filename
    size_t fname_len = wcstombs(NULL, filename, 0);
    if (fname_len == (size_t)-1) {
        return NULL;
    }
    
    char *mbfilename = malloc(fname_len + 1);
    if (mbfilename == NULL) {
        errno = ENOMEM;
        return NULL;
    }
    
    wcstombs(mbfilename, filename, fname_len + 1);
    
    // Convert mode
    size_t mode_len = wcstombs(NULL, mode, 0);
    if (mode_len == (size_t)-1) {
        free(mbfilename);
        return NULL;
    }
    
    char *mbmode = malloc(mode_len + 1);
    if (mbmode == NULL) {
        free(mbfilename);
        errno = ENOMEM;
        return NULL;
    }
    
    wcstombs(mbmode, mode, mode_len + 1);
    
    FILE *result = fopen(mbfilename, mbmode);
    
    free(mbfilename);
    free(mbmode);
    
    return result;
}

/**
 * @brief Remove file with wide character filename (Linux implementation)
 * @param filename Wide character filename
 * @return 0 on success, -1 on error
 */
int _wremove(const wchar_t *filename)
{
    if (filename == NULL) {
        errno = EINVAL;
        return -1;
    }
    
    // Convert filename
    size_t len = wcstombs(NULL, filename, 0);
    if (len == (size_t)-1) {
        return -1;
    }
    
    char *mbfilename = malloc(len + 1);
    if (mbfilename == NULL) {
        errno = ENOMEM;
        return -1;
    }
    
    wcstombs(mbfilename, filename, len + 1);
    
    int result = remove(mbfilename);
    
    free(mbfilename);
    
    return result;
}

/**
 * @brief Rename file with wide character filenames (Linux implementation)
 * @param oldname Wide character old filename
 * @param newname Wide character new filename
 * @return 0 on success, -1 on error
 */
int _wrename(const wchar_t *oldname, const wchar_t *newname)
{
    if (oldname == NULL || newname == NULL) {
        errno = EINVAL;
        return -1;
    }
    
    // Convert old filename
    size_t old_len = wcstombs(NULL, oldname, 0);
    if (old_len == (size_t)-1) {
        return -1;
    }
    
    char *mboldname = malloc(old_len + 1);
    if (mboldname == NULL) {
        errno = ENOMEM;
        return -1;
    }
    
    wcstombs(mboldname, oldname, old_len + 1);
    
    // Convert new filename
    size_t new_len = wcstombs(NULL, newname, 0);
    if (new_len == (size_t)-1) {
        free(mboldname);
        return -1;
    }
    
    char *mbnewname = malloc(new_len + 1);
    if (mbnewname == NULL) {
        free(mboldname);
        errno = ENOMEM;
        return -1;
    }
    
    wcstombs(mbnewname, newname, new_len + 1);
    
    int result = rename(mboldname, mbnewname);
    
    free(mboldname);
    free(mbnewname);
    
    return result;
}

#endif // !_WIN32