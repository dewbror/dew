/*
 * dewargs.h - small header-only CLI argument parsing library for C and C++
 * version 0.1
 *
 * Do this:
 *     #define DEWARGS_IMPLEMENTATION
 * before including this file in *one* C or C++ file to create the implementation.
 *
 * Example:
 *     #define DEWARGS_IMPLEMENTATION
 *     #include "dewargs.h"
 *
 *     int main(int argc, char **argv) {
 *         dewargs_init(argc, argv);
 *
 *         if (dewargs_has("--vsync", NULL))
 *             enable_vsync();
 *
 *         int width = dewargs_geti("--width", "-w", 1280);
 *         int height = dewargs_geti("--height", "-h", 720);
 *         const char *log = dewargs_getstr("--log", "-l", "app.log");
 *
 *         return 0;
 *     }
 *
 * Supported forms:
 *     --flag           (boolean, presence = true)
 *     --flag value     (value in next argv slot)
 *     --flag=value     (value joined with '=')
 *
 * License:
 *     MIT. See the end of this file.
 */

#ifndef DEWARGS_H_
#define DEWARGS_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dewargs_t
{
    int argc;
    char **argv;
} dewargs_t;

/* Initialize a 'dewargs_t' struct with the program's argc/argv.
 *
 * \param[out] a The 'dewargs_t' struct to initialize.
 * \param[in] argc The programs 'argc'.
 * \param[in] argv The programs 'argv'.
 */
void dewargs_init(dewargs_t *a, int argc, char **argv);

/* Return 1 if argl/args is present in argv, 0 otherwise. */
int dewargs_has(const char *argl, const char *args);

/* Return the string value associated with argl/args, or fallback if absent or malformed. */
const char *dewargs_getstr(const char *argl, const char *args, const char *fallback);

/* Return the integer value associated with argl/args, or fallback if absent or unparsable. */
int dewargs_geti(const char *argl, const char *args, int fallback);

/* Return the floating-point value associated with argl/args, or fallback if absent or unparsable. */
double dewargs_getf(const char *argl, const char *args, double fallback);

#ifdef DEWARGS_IMPLEMENTATION

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

void dewargs_init(dewargs_t *a, int argc, char **argv)
{
    a->argc = argc;
    a->argv = argv;
}

static int dewargs_match_(const char *arg, const char *option)
{
    if(option == NULL)
        return 0;

    size_t arg_len = strlen(arg);

    if(strncmp(arg, option, arg_len) != 0)
        return 0;

    // Match only if arg ends here, or has '=' immediately after.
    return arg[arg_len] == '\0' || arg[arg_len] == '=';
}

int dewargs_has(const char *long_option, const char *short_option)
{
    // TODO: scan argv for an exact match or a "arg=..." match
    (void)argl;
    (void)args;

    

    return 0;
}

const char *dewargs_getstr(const char *argl, const char *args, const char *fallback)
{
    // TODO: scan argv for "arg value" or "arg=value", return matching string
    (void)argl;
    (void)args;
    return fallback;
}

int dewargs_geti(const char *argl, const char *args, int fallback)
{
    // TODO: use dewargs_gettr + strtol with endptr validation
    (void)argl;
    (void)args;
    return fallback;
}

double dewargs_getf(const char *argl, const char *args, double fallback)
{
    // TODO: use dewargs_gettr + strtod with endptr validation
    (void)argl;
    (void)args;
    return fallback;
}

#endif // DEWARGS_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // DEWARGS_H_

/* Version History:
 *     0.1 (2026-05-30) Initial skeleton.
 */
/* Copyright (c) 2026 dewbror <dewbror@gmail.com>
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
