#ifndef _ZOS_API_H
#define _ZOS_API_H
//==============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "zlog_api.h"

//==============================================================================
#define ZOS_MAX_PROCESS_TIME       2000 // milli-second
#define BUFFER_SIZE         2048
#define SYNC                0
#define NORMAL              1

#ifdef ZCFG_MEMORY_LEAK_DETECT
#define ZCFG_ALLOC_RECORD_LEN 64
void dumpZcfgAllocRecord();
extern void dumpZcfgAllocFrontendRecord();
void dumpAllocRecord();
void recordAllocData(char *, char *, int);
void recordZcfgAllocData(void *, const char *, int, int);
void _ZOS_FREE(void*);
void* _ZOS_MALLOC(int, const char *, int);
#endif

//==============================================================================
/*
    Public MACRO
*/
#ifdef ZCFG_MEMORY_LEAK_DETECT
#define ZOS_MALLOC(_size_) _ZOS_MALLOC(_size_, __FILE__, __LINE__)
#else
#define ZOS_MALLOC(_size_) \
    ({\
        void *_p_ = NULL; \
        do \
        { \
            _p_ = calloc(1, (size_t)(_size_)); \
            if (_p_ == NULL) \
            { \
                ZLOG_ERROR("fail to calloc memory, size = %zu", (size_t)(_size_)); \
            } \
        } \
        while (0); \
        _p_; \
    })
#endif

#ifdef ZCFG_MEMORY_LEAK_DETECT
#define ZOS_FREE(_p_) do{_ZOS_FREE((void *)_p_);_p_=NULL;}while(0)
#else
#define ZOS_FREE(_p_) \
    {\
        do \
        { \
            if ((_p_) == NULL) \
            { \
                ZLOG_DEBUG("Free NULL pointer"); \
            } \
            else \
            { \
                free((void *)(_p_)); \
                (_p_) = NULL; \
            } \
        } \
        while (0); \
    }
#endif

#define ZOS_SUCCESS	0
#define ZOS_FAIL	-1

/*
    use system()
*/
#define ZOS_SYSTEM(_fmt_, ...) \
    ({ \
        int    zb = zos_system(true, _fmt_, ##__VA_ARGS__); \
        if (zb == ZOS_FAIL) \
        { \
            ZLOG_ERROR("fail to run system()"); \
        } \
        zb; \
    })

/*
    use zcfg_system() for security
*/
#define ZOS_SYSTEM_S(_fmt_, ...) \
    ({ \
        int    zb = zos_system(false, _fmt_, ##__VA_ARGS__); \
        if (zb == ZOS_FAIL) \
        { \
            ZLOG_ERROR("fail to run system()"); \
        } \
        zb; \
    })


#define ZOS_STRDUP(str) _zos_strdup(str, __FILE__, __LINE__)
#define ZOS_STRNCPY(d, s, l) do{strncpy(d, s, l-1);*(d+l-1) = '\0';}while(0)
#define ZOS_STRCAT(dst, src, bufferSize) zos_strcat(dst, src, bufferSize)
#define ZOS_MOVE_FILE(dst, src)       zos_copyFile(dst, src, MOVE)
#define ZOS_COPY_FILE(dst, src)       zos_copyFile(dst, src, COPY)
#define ZOS_COPY_LINK_FILE(dst, src)  zos_copyFile(dst, src, COPY_LINK)
#define ZOS_REMOVE_DIR(dirPath)       zos_removeDir(dirPath)
#define ZOS_MKDIR(dirPath)            zos_mkdir(dirPath)
#define ZOS_TOUCH_FILE(fileName)      zos_touchFile(fileName)
#define ZOS_FCLOSE_S(fp)              zos_fclose(fp, SYNC)
#define ZOS_FCLOSE(fp)                zos_fclose(fp, NORMAL)
#define ZOS_FOPEN(fileName, mode)     fopen(fileName, mode)

typedef enum
{
    MOVE,
    COPY,
	COPY_LINK,
} file_action;

//==============================================================================
/*!
 *  system command
 *
 *  @param [in] b_linux     if true then use linux system() else use _zcfg_system()
 *  @param [in] format      string format of command
 *  @param [in] ...         arguments required by the format
 *
 *  @return ZOS_SUCCESS  - command executed successful
 *          ZOS_FAIL     - fail to execute command
 */
bool zos_system(
    bool        b_linux,
    const char  *format,
    ...
);

/*!
 *  work as snprintf(), avoid the issue from man page
 *      "However, the standards explicitly note that the results are undefined
 *      if source and destination buffers overlap when calling sprintf(),
 *      snprintf(), vsprintf(), and vsnprintf()."
 *
 *  @param [in] buf          string buffer to print to
 *  @param [in[ size         maximum size of the string
 *  @param [in] format       string format of command
 *  @param [in] ...          arguments required by the format
 *
 *  @return >= 0 - successful, the number of characters printed
 *          <  0 - failed
 */
int zos_snprintf(
    char        *buf,
    size_t      size,
    const char  *format,
    ...
);

/*!
 *  get my pid which is shown in top command.
 *
 *  @return uint32_t      my pid
 */
uint32_t zos_pid_get();

/*!
 *  get monotonic time in milli-second.
 *
 *  @return uint32_t    milli-second
 *                      0: failed
 */
uint32_t zos_mtime_get();

/*!
 *  
 * Appends a copy of the source string to the destination string.
 * The terminating null character in destination is overwritten by the first character of source,
 * and a null-character is included at the end of the new string formed by the concatenation of both in destination.
 * Destination and source shall not overlap.
 *  
 *  @param[in] char *destination         Pointer to the destination array, which should contain a C string, and be large enough to contain the concatenated resulting string.
 *  @param[in] const char *source      C string to be appended. This should not overlap destination.
 *  @param[in] bufferSize                   Size of the destination array.
 *  @return                           ZOS_SUCCESS: 0, ZOS_FAIL: -1
 */
int zos_strcat(char *destination, const char *source, size_t bufferSize);


/*!
 *  Move or copy the source to destination.
 *  Both source and destination should be files.
 *  If the source file is a symbolic link, COPY will copy the file it points to,
 *  and COPY_LINK will copy the symbolic link itself.
 *
 *  @param[in] const char *dst        Destination file name.
 *  @param[in] const char *src        Source file name.
 *  @param[in] file_action action     MOVE, COPY or COPY_LINK.
 *  @return                           ZOS_SUCCESS: 0, ZOS_FAIL: -1
 */
int zos_copyFile(char *dst, char *src, file_action action);

/*!
 *  Remove the directory.
 *
 *  @param[in] const char *dirPath    Path of the directory.
 *  @return                           ZOS_SUCCESS: 0, ZOS_FAIL: -1
 */
int zos_removeDir(const char *dirPath);

/*!
 *  Create a directory.
 *
 *  @param[in] const char *dirPath    Path of the directory.
 *  @return                           ZOS_SUCCESS: 0, ZOS_FAIL: -1
 */
int zos_mkdir(const char *dirPath);

/*!
 *  Create a file.
 *
 *  @param[in] char *fileName         Name of the file.
 *  @return                           ZOS_SUCCESS: 0, ZOS_FAIL: -1
 */
int zos_touchFile(char *fileName);

/*!
 *  If the type is SYNC, ensure the buffered data are written into the file and disk then do fclose.
 *  If the type is NORMAL, just do fclose.
 *
 *  @param[in] FILE *fp               Pointer to a FILE structure.
 *  @param[in] int type               SYNC or NORMAL.
 *  @return                           ZOS_SUCCESS: 0, ZOS_FAIL: -1
 */
int zos_fclose(FILE *fp, int type);

/*!
 *  The same function to strdup, but able to trace memory allocation record
 *  Need use macro ZOS_STRDUP, but not _zos_strdup
 */
char *_zos_strdup(const char *, const char *, int);

//==============================================================================
#endif /* _ZOS_API_H */
