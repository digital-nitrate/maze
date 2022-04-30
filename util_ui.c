/* Implementation of terminal dialog
 * utility functions for reading basic
 * values from user.
 */

#include <stddef.h>
#include <stdlib.h>

#include <ncurses.h>

#include "util_ui.h"
#include "file_env.h"

#define EXPANPATH 128

#ifndef NULL
#define NULL ((void*)0)
#endif

#undef __attribute__
#ifdef __GNUC__
#define __attribute__(args) __attribute__(args)
#else
#define __attribute__(args)
#endif

unsigned int get_uint(size_t digit_count, int y_exts, int y_exte, attr_t norm_attrs, int norm_pair, attr_t err_attrs, int err_pair, int* empty) {
	size_t loc = 0;
	size_t length = 0;
	char buffer[digit_count + 1];
	int y;
	int x;
	attr_set(norm_attrs, norm_pair, NULL);
	getyx(stdscr, y, x);
	for (size_t i = 0; i < digit_count; ++i) {
		addch(' ');
	} // Clear dialog space
	for (int i = y_exts; i < y_exte; ++i) {
		move(i, 0);
		clrtoeol();
	} // Clear information space
	curs_set(1); // Set cursor to visible
	move(y, x); // Correct position
	refresh(); // Update screen
	int c;
	while ((c = getch()) != '\n') {
		for (int i = y_exts; i < y_exte; ++i) {
			move(i, 0);
			clrtoeol();
		} // Clear information space
		switch (c) {
			case KEY_RESIZE:
				endwin();
				exit(-2);
			case KEY_BACKSPACE:
				if (loc == 0) {
					attr_set(err_attrs, err_pair, NULL);
					mvaddstr(y_exts, 0, "No Digit To Delete");
					beep();
				} else {
					move(y, x + loc - 1);
					for (size_t i = loc - 1; i < length - 1; ++i) {
						buffer[i] = buffer[i + 1];
						addch(buffer[i]);
					}
					addch(' ');
					--loc;
					--length;
				}
				break;
			case KEY_LEFT:
				if (loc == 0) {
					attr_set(err_attrs, err_pair, NULL);
					mvaddstr(y_exts, 0, "Cannot Move Left");
					beep();
				} else {
					--loc;
				}
				break;
			case KEY_RIGHT:
				if (loc == length) {
					attr_set(err_attrs, err_pair, NULL);
					mvaddstr(y_exts, 0, "Cannot Move Right");
					beep();
				} else {
					++loc;
				}
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (length == digit_count) {
					attr_set(err_attrs, err_pair, NULL);
					mvaddstr(y_exts, 0, "Buffer Full");
					beep();
				} else {
					move(y, x + loc + 1);
					for (size_t i = length; i > loc; --i) {
						buffer[i] = buffer[i - 1];
					}
					for (size_t i = loc; i < length; ++i) {
						addch(buffer[i + 1]);
					}
					mvaddch(y, x + loc, c);
					buffer[loc] = c;
					++loc;
					++length;
				}
				break;
			default:
				attr_set(err_attrs, err_pair, NULL);
				mvaddstr(y_exts, 0, "Expected A Digit Character");
				beep();
		}
		move(y, x + loc);
		attr_set(norm_attrs, norm_pair, NULL);
		refresh();
	}
	for (int i = y_exts; i < y_exte; ++i) {
		move(i, 0);
		clrtoeol();
	} // Clear information space
	buffer[length] = '\0';
	if (empty != NULL) *empty = (length == 0) ? 1 : 0;
	return atoi(buffer); // Convert buffer into an unsigned integer
}

size_t get_str(char* buffer, size_t buffer_size, int y_exts, int y_exte, attr_t norm_attrs, int norm_pair, attr_t err_attrs, int err_pair) {
	size_t loc = 0;
	size_t length = 0;
	int y;
	int x;
	int maxy __attribute__((unused));
	int maxx;
	getmaxyx(stdscr, maxy, maxx); // Get screen dimensions
	getyx(stdscr, y, x); // Get location
	attr_set(norm_attrs, norm_pair, NULL);
	for (size_t i = 0; i < buffer_size; ++i) {
		addch(' ');
	} // Clear input space
	for (int i = y_exts; i < y_exte; ++i) {
		move(i, 0);
		clrtoeol();
	} // Clear information space
	curs_set(1); // Set cursor to visible
	move(y, x); // Correct position
	refresh(); // Update screen
	int c;
	while ((c = getch()) != '\n') {
		for (int i = y_exts; i < y_exte; ++i) {
			move(i, 0);
			clrtoeol();
		} // Clear information space
		switch (c) {
			case KEY_RESIZE:
				endwin();
				exit(-2);
			case KEY_BACKSPACE:
				if (loc == 0) {
					attr_set(err_attrs, err_pair, NULL);
					mvaddstr(y_exts, 0, "No Character To Delete");
					beep();
				} else {
					move(y, x + loc - 1);
					for (size_t i = loc - 1; i < length - 1; ++i) {
						buffer[i] = buffer[i + 1];
						addch(buffer[i]);
					}
					addch(' ');
					--loc;
					--length;
				}
				break;
			case KEY_LEFT:
				if (loc == 0) {
					attr_set(err_attrs, err_pair, NULL);
					mvaddstr(y_exts, 0, "Cannot Move Left");
					beep();
				} else {
					--loc;
				}
				break;
			case KEY_RIGHT:
				if (loc == length) {
					attr_set(err_attrs, err_pair, NULL);
					mvaddstr(y_exts, 0, "Cannot Move Right");
					beep();
				} else {
					++loc;
				}
				break;
			case '\t':
			{
				move(y_exts, 0);
				clrtoeol();
				buffer[length] = '\0';
				size_t matches;
				size_t new_length;
				char tmp[maxx * (y_exte - y_exts)];
				if (buffer[0] == '~') { // Check if tilde expansion is needed
					char expanded[EXPANPATH + buffer_size - 1];
					char* const end = buffer + length;
					char* curr = buffer + 1;
					char* exp_curr = expanded;
					while (curr != end && *curr != '/') {
						*exp_curr = *curr;
						++exp_curr;
						++curr;
					}
					char* const tilde_loc = curr;
					if (*curr == '/') { // Check if user is entered
						*exp_curr = '\0';
						size_t ext_len = tilde_exp(expanded, expanded, EXPANPATH);
						exp_curr = expanded + ext_len;
						while (curr != end) {
							*exp_curr = *curr;
							++exp_curr;
							++curr;
						}
						*exp_curr = '\0';
						matches = match_find(expanded, expanded, ext_len + (buffer + buffer_size - tilde_loc), tmp, maxx * (y_exte - y_exts), &new_length, NULL);
						char* const exp_end = expanded + new_length;
						new_length = new_length - ext_len + (tilde_loc - buffer);
						while (exp_curr != exp_end) {
							*curr = *exp_curr;
							++curr;
							++exp_curr;
						}
					} else {
						matches = tilde_find(buffer + 1, buffer + 1, buffer_size - 1, tmp, maxx * (y_exte - y_exts), &new_length, NULL);
						++new_length;
					}
				} else {
					matches = match_find(buffer, buffer, buffer_size, tmp, maxx * (y_exte - y_exts), &new_length, NULL);
				}
				if (new_length == length) {
					beep();
					if (matches != 1) {
						char* ptr = tmp;
						for (int i = y_exts; i < y_exte && *ptr != '\0'; ++i) {
							move(i, 0);
							for (int j = 0; j < maxx && *ptr != '\0'; ++j) {
								addch(*ptr);
								++ptr;
							}
						} // List matches if more than one
					}
				} else {
					loc = new_length;
					length = new_length;
					move(y, x);
					for (size_t i = 0; i < new_length; ++i) {
						addch(buffer[i]);
					} // Add characters onto screen
				}
			}
				break;
			default:
				if (c >= 32 && c < 127) {
					if (length == buffer_size - 1) {
						attr_set(err_attrs, err_pair, NULL);
						mvaddstr(y_exts, 0, "Buffer Full");
						beep();
					} else {
						move(y, x + loc + 1);
						for (size_t i = length; i > loc; --i) {
							buffer[i] = buffer[i - 1];
						}
						for (size_t i = loc; i < length; ++i) {
							addch(buffer[i + 1]);
						}
						mvaddch(y, x + loc, c);
						buffer[loc] = c;
						++loc;
						++length;
					}
				} else {
					attr_set(err_attrs, err_pair, NULL);
					mvaddstr(y_exts, 0, "Not A Valid Character");
					beep();
				}
		}
		move(y, x + loc);
		attr_set(norm_attrs, norm_pair, NULL);
		refresh();
	}
	for (int i = y_exts; i < y_exte; ++i) {
		move(i, 0);
		clrtoeol();
	} // Clear information space
	buffer[length] = '\0';
	return length;
}
