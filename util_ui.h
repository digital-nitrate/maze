#ifndef UTIL_UI_H
#define UTIL_UI_H 1

/* User Input Utility Header File
 *
 * Header File That Contains Utility
 * Functions For Getting A Pathname
 * And Unsigned Int From Terminal IO.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#include <ncurses.h>

#undef __attribute__
#ifdef __GNUC__
#define __attribute__(args) __attribute__(args)
#else
#define __attribute__(args)
#endif

/* Initiates a terminal dialog for getting
 * an unsigned integer. The number of digits
 * as well as terminal location for outputting
 * extra information are arguments. Furthermore,
 * coloring and style information is passed for
 * input and information. An optional argument
 * is added for saving whether the user inputed
 * nothing or the buffer was empty after the
 * dialog. This shall return an unsigned integer
 * based on the terminal input.
 */
unsigned int get_uint(size_t digit_count, int y_exts, int y_exte, attr_t norm_attrs, int norm_pair, attr_t err_attrs, int err_pair, int* empty);

/* Initiates a terminal dialog for getting a file
 * pathname. The size of the buffer and a pointer
 * to the output buffer are arguments. Furthermore,
 * terminal location for outputting extra information,
 * and coloring style information for the normal and
 * information text. It shall return the length of
 * the inputed path. One of the main features of this
 * dialog shall be that pressing tab shall outfill
 * the buffer based on possible file names. This
 * was built to mimic the tab completion in
 * ubuntu linux shell.
 */
size_t get_str(char* buffer, size_t buffer_size, int y_ext, int x_ext, attr_t norm_attrs, int norm_pair, attr_t err_attrs, int err_pair) __attribute__((nonnull(1)));

#ifdef __cplusplus
}
#endif

#endif
