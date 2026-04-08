/* dewlog.h - v0.3
 *
 * Documentation:
 * TODO: Write this...
 */

#ifndef DEWLOG_H_
#define DEWLOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define DEWLOG_LEVEL_ERROR 0
#define DEWLOG_LEVEL_WARN  1
#define DEWLOG_LEVEL_INFO  2
#define DEWLOG_LEVEL_DEBUG 3
#define DEWLOG_LEVEL_TRACE 4

/* Open file 'file_name' for logging, 'file_name' == NULL will set logging to stderr.
 *
 * \param[in] file_name A string with the name of the log file to open.
 */
void dewlog_open(const char *file_name);

/*
 * Close log file.
 */
void dewlog_close(void);

/* Check for GCC or Clang */
#if defined(__GNUC__) || defined(__clang__)
#define FORMAT_ATTR(format_index, first_arg_index) __attribute__((format(printf, format_index, first_arg_index)))
#else
#define FORMAT_ATTR(format_index, first_arg_index) /* NOP for MSVC and other compilers */
#endif

/* NOT MEANT TO BE USED! Use LOG_ERROR, _INFO, ... etc. instead.
 *
 * Valid DEWLOG_LEVELS are:
 *     0: ERROR,
 *     1: WARNING,
 *     2: INFO,
 *     3: DEBUG,
 *     4: TRACE.
 *
 * \param[in] level The DEWLOG_LEVEL of the message,
 * \param[in] fmt A printf-style message format string.
 * \param[in] ... Additional parameters matching % tokens in the "fmt" string, if any.
 */
void __dewlog__msg(const int level, const char *file, const int line, const char *func, const char *fmt, ...)
    FORMAT_ATTR(5, 6);

#undef FORMAT_ATTR

/* If DEWLOG_LEVEL is not defined, use default DEWLOG_LEVEL */
#ifndef DEWLOG_LEVEL
#ifdef NDEBUG
/* Default DEWLOG_LEVEL for NDEBUG mode is DEWLOG_LEVEL_INFO */
#define DEWLOG_LEVEL DEWLOG_LEVEL_INFO
#else
/* Default DEWLOG_LEVEL in debug mode is DEWLOG_LEVEL_TRACE */
#define DEWLOG_LEVEL DEWLOG_LEVEL_TRACE
#endif
#endif

#if DEWLOG_LEVEL >= DEWLOG_LEVEL_ERROR
#define LOG_ERROR(...) __dewlog__msg(DEWLOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define LOG_ERROR(...) ((void)0)
#endif

#if DEWLOG_LEVEL >= DEWLOG_LEVEL_WARN
#define LOG_WARN(...) __dewlog__msg(DEWLOG_LEVEL_WARN, __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define LOG_WARN(...) ((void)0)
#endif

#if DEWLOG_LEVEL >= DEWLOG_LEVEL_INFO
#define LOG_INFO(...) __dewlog__msg(DEWLOG_LEVEL_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define LOG_INFO(...) ((void)0)
#endif

#if DEWLOG_LEVEL >= DEWLOG_LEVEL_DEBUG
#define LOG_DEBUG(...) __dewlog__msg(DEWLOG_LEVEL_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define LOG_DEBUG(...) ((void)0)
#endif

#if DEWLOG_LEVEL >= DEWLOG_LEVEL_TRACE
#define LOG_TRACE(...) __dewlog__msg(DEWLOG_LEVEL_TRACE, __FILE__, __LINE__, __func__, __VA_ARGS__)
#else
#define LOG_TRACE(...) ((void)0)
#endif

#ifdef DEWLOG_IMPLEMENTATION

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#define LOGBUF_MAX 512

#define RED    "\x1b[31m"
#define YELLOW "\x1b[33m"
#define GREEN  "\x1b[32m"
#define BLUE   "\x1b[34m"
#define CYAN   "\x1b[36m"
#define RESET  "\x1b[0m"

/* TODO: Add thread safe access to static data (via #ifdef DEWLOG_THREAD_SAFE_*) */
static int DEWLOG_logging_to_file = 0;
static FILE *DEWLOG_fp = NULL;
static const char *DEWLOG_file_name = NULL;
static const char *DEWLOG_levels_plain[5] = {"[ERR] ", "[WRN] ", "[INF] ", "[DBG] ", "[TRC] "};
static const char *DEWLOG_levels_color[5] = {RED "[ERR]" RESET " ", YELLOW "[WRN]" RESET " ", GREEN "[INF]" RESET " ",
    BLUE "[DBG]" RESET " ", CYAN "[TRC]" RESET " "};
static time_t DEWLOG_last_time = 0;
static char DEWLOG_time_buf[21]; /* "YYYY-MM-DD HH:MM:SS \0" */
static size_t DEWLOG_time_buf_len = 0;

void dewlog_open(const char *p_file_name)
{
    if(DEWLOG_logging_to_file)
        return;

    if(p_file_name == NULL)
    {
        DEWLOG_logging_to_file = 0;
        DEWLOG_fp = stderr;

        /* stderr is unbuffered by default, meaning every fwrite() triggers an immediate
         * OS write() syscall. _IOLBF (line buffering) batches writes until a newline is
         * seen, so each complete log line costs one syscall instead of three.
         */
        setvbuf(DEWLOG_fp, NULL, _IOLBF, 0);
        return;
    }

    DEWLOG_fp = fopen(p_file_name, "w");
    if(DEWLOG_fp == NULL)
        return;

    /* File streams are usually fully buffered by default, but we make it explicit
     * and set a large buffer (65536 = 64KB) to minimize OS write() syscalls.
     * Unlike stderr, there is no automatic flush on newline (_IOFBF) -- writes
     * accumulate in the buffer until it fills or fflush() is called. This means
     * log lines written near a crash may be lost; see the flush-on-error behavior
     * in __dewlog__msg to mitigate this for high-severity messages. 
     */
    setvbuf(DEWLOG_fp, NULL, _IOFBF, 65536);

    DEWLOG_file_name = p_file_name;
    DEWLOG_logging_to_file = 1;

    LOG_INFO("Log file opened '%s'", DEWLOG_file_name);
}

void dewlog_close(void)
{
    if(!DEWLOG_logging_to_file)
        return;

    LOG_DEBUG("Attempting to close log file '%s'", DEWLOG_file_name);

    /* Check if log_file is NULL */
    if(DEWLOG_fp == NULL)
        return;

    /* Close log_file */
    fflush(DEWLOG_fp);
    int ret = fclose(DEWLOG_fp);
    if(ret != 0)
    {
        LOG_DEBUG("Failed to close log file '%s'", DEWLOG_file_name);
        return;
    }

    /* Set file ptr to default stderr */
    DEWLOG_fp = stderr;
    DEWLOG_logging_to_file = 0;
}

void __dewlog__msg(const int level, const char *file, const int line, const char *func, const char *fmt, ...)
{
    /* Null check fp_log */
    if(DEWLOG_fp == NULL)
        return;

#if defined(DEWLOG_NO_FILE)
    (void)file;
#endif
#if defined(DEWLOG_NO_LINE)
    (void)line;
#endif
#if defined(DEWLOG_NO_FUNC)
    (void)func;
#endif

    int iret = 0;
    size_t sret = 0;
    char logbuf[LOGBUF_MAX];

    time_t now = time(NULL);
    if(now != DEWLOG_last_time)
    {
        struct tm *tm_now = localtime(&now);
        DEWLOG_time_buf_len = strftime(DEWLOG_time_buf, sizeof(DEWLOG_time_buf), "%Y-%m-%d %H:%M:%S ", tm_now);
        DEWLOG_last_time = now;
    }

    if(DEWLOG_time_buf_len > 0 && DEWLOG_time_buf_len < sizeof(logbuf))
    {
        memcpy(logbuf, DEWLOG_time_buf, DEWLOG_time_buf_len);
        sret = DEWLOG_time_buf_len;
    }

#ifndef DEWLOG_FILE_MAX_LEN
#define DEWLOG_FILE_MAX_LEN 35
#endif

#ifndef DEWLOG_FUNC_MAX_LEN
#define DEWLOG_FUNC_MAX_LEN 25
#endif

#ifndef DEWLOG_NO_FILE
    size_t file_len = strlen(file);
    if(file_len > DEWLOG_FILE_MAX_LEN)
    {
        iret = snprintf(logbuf + sret, sizeof(logbuf) - sret, "...%*s", DEWLOG_FILE_MAX_LEN - 3,
            file + file_len - (DEWLOG_FILE_MAX_LEN - 3));
    }
    else
    {
        iret = snprintf(logbuf + sret, sizeof(logbuf) - sret, "%*s", DEWLOG_FILE_MAX_LEN, file);
    }
    if(iret > 0)
        sret += (size_t)iret;
#endif
#ifndef DEWLOG_NO_LINE
    iret = snprintf(logbuf + sret, sizeof(logbuf) - sret, ":%4d ", line);
    if(iret > 0)
        sret += (size_t)iret;
#endif
#ifndef DEWLOG_NO_FUNC
    size_t func_len = strlen(func);
    if(func_len > DEWLOG_FUNC_MAX_LEN)
    {
        iret = snprintf(logbuf + sret, sizeof(logbuf) - sret, "[...%*s]", DEWLOG_FUNC_MAX_LEN - 3,
            func + func_len - (DEWLOG_FUNC_MAX_LEN - 3));
    }
    else
    {
        iret = snprintf(logbuf + sret, sizeof(logbuf) - sret, "[%*s]", DEWLOG_FUNC_MAX_LEN, func);
    }
    if(iret > 0)
        sret += (size_t)iret;
#endif

#undef DEWLOG_FILE_MAX_LEN
#undef DEWLOG_FUNC_MAX_LEN

    int safe_level = (level >= 0 && level < 5) ? level : 4;
    const char *level_str = DEWLOG_logging_to_file ? DEWLOG_levels_plain[safe_level] : DEWLOG_levels_color[safe_level];

    size_t level_str_len = strlen(level_str);
    if(sret + level_str_len < sizeof(logbuf))
    {
        memcpy(logbuf + sret, level_str, level_str_len);
        sret += level_str_len;
    }

    if(fmt)
    {
        va_list args;
        va_start(args, fmt);
        iret = vsnprintf(logbuf + sret, sizeof(logbuf) - sret, fmt, args);
        va_end(args);
        if(iret > 0)
            sret += (size_t)iret;
    }

    /* Clamp sret and append newline, overwriting last byte if buffer is full */
    if(sret >= sizeof(logbuf))
        sret = sizeof(logbuf) - 1;
    logbuf[sret++] = '\n';

    fwrite(logbuf, 1, sret, DEWLOG_fp);

    /* Flush immediately for high severity messages. Under _IOFBF file buffering,
     * log lines near a crash may get lost in the buffer. ERROR and WARN
     * are infrequent enough that the syscall cost here doesn't matter. */
    if(level <= DEWLOG_LEVEL_WARN)
        fflush(DEWLOG_fp);
}

/* Undef macros */
#undef RED
#undef YELLOW
#undef GREEN
#undef BLUE
#undef CYAN
#undef RESET
#undef LOGBUF_MAX

#endif /* DEWLOG_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* DEWLOG_H_ */

/* Version History:
 *     0.4 (2026-04-08) Performance improvements:
 *                      Timestamp caching: localtime() and strftime() are now only called once per
 *                      second, subsequent calls within the same second reuse the cached string.
 *                      Level prefix strings moved to file-scope statics, appended via memcpy.
 *                      Full message assembled into a single buffer and written with one fwrite().
 *                      setvbuf() set to _IOLBF on stderr and _IOFBF (64KB) on file streams to
 *                      reduce OS write() syscalls. ERROR and WARN messages flush immediately to
 *                      preserve log lines near a crash.
 *     0.3 (2026-02-08) Added macros DEWLOG_NO_FILE, DEWLOG_NO_LINE and DEWLOG_NO_FUNC to disable printing of file name,
 *                      line number and function name.
 *                      DEWLOG_FILE_MAX_LEN and DEWLOG_FUNC_MAX_LEN macros added to limit the length of file and
 *                      function name. Default values are
 *                         - DEWLOG_FILE_MAX_LEN 35
 *                         - DEWLOG_FUNC_MAX_LEN 25
 *     0.2 (2026-02-07) Changed name c_log -> dewlog.
 *     0.1 (2025-10-01) First released version.
 */

/* Copyright (c) 2025 dewbror <dewbror@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the “Software”), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
