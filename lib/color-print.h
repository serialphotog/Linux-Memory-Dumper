/**
 * color-print.h
 *
 * This is an extremely simple, single header library for printing messages
 * to the console in color. It simplifies the process of using ANSI terminal
 * color codes for printing messages as a matter of convenience.
 *
 * Author: Adam Thompson (https://hackeradam.com).
 *
 * The latest version of this library can be found on GitHub:
 * https://github.com/serialphotog/color-print.h
*/

/*
   This is free and unencumbered software released into the public domain.

   Anyone is free to copy, modify, publish, use, compile, sell, or
   distribute this software, either in source code form or as a compiled
   binary, for any purpose, commercial or non-commercial, and by any
   means.

   In jurisdictions that recognize copyright laws, the author or authors
   of this software dedicate any and all copyright interest in the
   software to the public domain. We make this dedication for the benefit
   of the public at large and to the detriment of our heirs and
   successors. We intend this dedication to be an overt act of
   relinquishment in perpetuity of all present and future rights to this
   software under copyright law.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.

   For more information, please refer to <http://unlicense.org/>.
*/

#ifndef __COLOR_PRINT_H__
#define __COLOR_PRINT_H__

#include <stdarg.h>
#include <stdio.h>

// Color definitions
#define CLI_COLOR_CLEAR     "\033[31m"
#define CLI_COLOR_RED       "\033[31m"
#define CLI_COLOR_GREEN     "\033[32m"
#define CLI_COLOR_YELLOW    "\033[33m"
#define CLI_COLOR_BLUE      "\033[34m"
#define CLI_COLOR_PURPLE    "\033[35m"
#define CLI_COLOR_CYAN      "\033[36m"
#define CLI_COLOR_GREY      "\033[37m"

// Macro that generates the helper functions for printing colored messages to
// the console.
#define DEFINE_COLOR_PRINT(name, color)                         \
    static inline void print_##name(const char *format, ...) {                \
        va_list args;                                           \
        va_start(args, format);                                 \
        vprint_color(stdout, color, format, args);              \
        va_end(args);                                           \
    }                                                           \
    static inline void fprint_##name(FILE *stream, const char *format, ...) { \
        va_list args;                                           \
        va_start(args, format);                                 \
        vprint_color(stream, color, format, args);              \
        va_end(args);                                           \
    }

/**
 * The internal helper that performs the actual printing of colored messages
 * to the console.
 *
 * Args:
 *      - const char *color: The ANSI color escape sequence to use.
 *      - const char *format: The format string to use for printing.
 *      - va_list args: The additional arguments to pass to printf.
*/
static inline void vprint_color(FILE *stream, const char *color, const char *format, va_list args)
{
    fprintf(stream, "%s", color);
    vfprintf(stream, format, args);
    fprintf(stream, "%s", CLI_COLOR_CLEAR);
}

/**
 * Print a message using a color definition.
 *
 * Args:
 *      - const char *color: The ANSI color escape sequence to use.
 *      - const char *format: The format string to use for printing.
 *      - ... The remaining arguments to pass to `printf`.
*/
static inline void print_color(const char *color, const char *format, ...) 
{
    va_list args;
    va_start(args, format);
    vprint_color(stdout, color, format, args);
    va_end(args);
}

/**
 * Prints a message using a given color definition to the given file descriptor.
 *
 * Args:
 *      - const char *color: The ANSI color escape sequence to use.
 *      - const char *format: The format string to use for printing.
 *      - ... The remaining arguments to pass to `printf`.
*/
static inline void fprint_color(FILE *stream, const char *color, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprint_color(stream, color, format, args);
    va_end(args);
}

// Generate the helper functions for printing a message in a given color.
DEFINE_COLOR_PRINT(red, CLI_COLOR_RED)
DEFINE_COLOR_PRINT(green, CLI_COLOR_GREEN)
DEFINE_COLOR_PRINT(yellow, CLI_COLOR_YELLOW)
DEFINE_COLOR_PRINT(blue, CLI_COLOR_BLUE)
DEFINE_COLOR_PRINT(purple, CLI_COLOR_PURPLE)
DEFINE_COLOR_PRINT(cyan, CLI_COLOR_CYAN)
DEFINE_COLOR_PRINT(grey, CLI_COLOR_GREY)

#endif