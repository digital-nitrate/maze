#ifndef UTIL_UI_H
#define UTIL_UI_H 1

#include <stddef.h>

#include <ncurses.h>

unsigned int get_uint(size_t digit_count, int y_exts, int y_exte, attr_t norm_attrs, int norm_pair, attr_t err_attrs, int err_pair, int* empty);

size_t get_str(char* buffer, size_t buffer_size, int y_ext, int x_ext, attr_t norm_attrs, int norm_pair, attr_t err_attrs, int err_pair);

#endif
