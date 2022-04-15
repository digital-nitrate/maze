#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ncurses.h>

#include "maze.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

#define BUFFERSIZE 16

char const* const top_msg = "Welcome To Mad Maze Solution Visualizer";
char const* const nopath_msg = "No Path Found";

static inline size_t get_file(char* buffer, size_t buffer_size) {
	size_t loc = 0;
	size_t length = 0;
	attr_set(A_NORMAL, 0, NULL);
	addstr("Enter Maze File: ");
	int y;
	int x;
	getyx(stdscr, y, x);
	for (size_t i = 0; i < buffer_size; ++i) {
		addch(' ');
	}
	curs_set(1);
	move(y, x);
	refresh();
	int c;
	while ((c = getch()) != '\n') {
		switch (c) {
			case KEY_BACKSPACE:
				if (loc == 0) {
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
					beep();
				} else {
					--loc;
				}
				break;
			case KEY_RIGHT:
				if (loc == length) {
					beep();
				} else {
					++loc;
				}
				break;
			default:
				if (c >= 32 && c < 127) {
					if (length == buffer_size - 1) {
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
				}
		}
		move(y, x + loc);
		refresh();
	}
	buffer[length] = '\0';
	return length;
}

static inline int visualize_maze(maze m, uiloc* path, unsigned int path_count) {
	int y;
	int x;
	getyx(stdscr, y, x);
	curs_set(0);
	if (path_count == 0) {
		size_t len = strlen(nopath_msg);
		attr_set(A_BOLD, 0, NULL);
		mvaddstr(y, x - (len >> 1), nopath_msg);
		int c;
		while ((c = getch()) != '\n') {}
		return 0;
	}
	size_t curr = 0;
	unsigned char* visits = malloc(sizeof(unsigned char) * m.dim.r * m.dim.c);
	if (visits == NULL) return -1;
	unsigned char* const end = visits + (m.dim.r * m.dim.c);
	for (unsigned char* ptr = visits; ptr != end; ++ptr) {
		*ptr = 0;
	}
	for (unsigned int i = 0; i < m.dim.r; ++i) {
		for (unsigned int j = 0; j < m.dim.c; ++j) {
			attr_set((m.data[i * m.dim.c + j].d) ? A_UNDERLINE : A_NORMAL, 0, NULL);
			mvprintw(y - m.dim.r + 1 + (i << 1), x - (m.dim.c << 1) + 1 + (j << 2), "%u", m.data[i * m.dim.c + j].len);
		}
	}
	int c;
	uiloc mid = *path;
	++(visits[mid.r * m.dim.c + mid.c]);
	attr_set((m.data[mid.r * m.dim.c + mid.c].d) ? A_UNDERLINE : A_NORMAL, 3, NULL);
	mvprintw(y - m.dim.r + 1 + (mid.r << 1), x - (m.dim.c << 1) + 1 + (mid.c << 1), "%u", m.data[mid.r * m.dim.c + mid.c].len);
	refresh();
	while ((c = getch()) != '\n') {
		switch(c) {
			case KEY_RIGHT:
				if (curr == path_count - 1) {
					beep();
				} else {
					unsigned int ind = mid.r * m.dim.c + mid.c;
					attr_set((m.data[ind].d) ? A_UNDERLINE : A_NORMAL, visits[ind], NULL);
					mvprintw(y - m.dim.r + 1 + (mid.r << 1), x - (m.dim.c << 1) + 1 + (mid.c << 2), "%u", m.data[ind].len);
					++curr;
					mid = path[curr];
					ind = mid.r * m.dim.c + mid.c;
					attr_set((m.data[ind].d) ? A_UNDERLINE : A_NORMAL, 3, NULL);
					mvprintw(y - m.dim.r + 1 + (mid.r << 1), x - (m.dim.c << 1) + 1 + (mid.c << 2), "%u", m.data[ind].len);
					++(visits[ind]);
				}
				break;
			case KEY_LEFT:
				if (curr == 0) {
					beep();
				} else {
					unsigned int ind = mid.r * m.dim.c + mid.c;
					--(visits[ind]);
					attr_set((m.data[ind].d) ? A_UNDERLINE : A_NORMAL, visits[ind], NULL);
					mvprintw(y - m.dim.r + 1 + (mid.r << 1), x - (m.dim.c << 1) + 1 + (mid.c << 2), "%u", m.data[ind].len);
					--curr;
					mid = path[curr];
					ind = mid.r * m.dim.c + mid.c;
					attr_set((m.data[ind].d) ? A_UNDERLINE : A_NORMAL, 3, NULL);
					mvprintw(y - m.dim.r + 1 + (mid.r << 1), x - (m.dim.c << 1) + 1 + (mid.c << 2), "%u", m.data[ind].len);
				}
				break;
		}
		refresh();
	}
	free(visits);
	return 0;
}

int main(void) {
	char buffer[BUFFERSIZE];
	size_t top_len = strlen(top_msg);
	initscr();
	start_color();
	noecho();
	keypad(stdscr, 1);
	cbreak();
	init_pair(0, COLOR_WHITE, COLOR_BLACK);
	init_pair(1, COLOR_WHITE, COLOR_GREEN);
	init_pair(2, COLOR_WHITE, COLOR_RED);
	init_pair(3, COLOR_WHITE, COLOR_CYAN);
	init_pair(4, COLOR_RED, COLOR_BLACK);
	attr_set(A_BOLD | A_UNDERLINE, 0, NULL);
	int size_y;
	int size_x;
	getmaxyx(stdscr, size_y, size_x);
	mvaddstr(0, (size_x - top_len) >> 1, top_msg);
	refresh();
	while (1) {
		move(size_y - 1, 0);
		size_t len = get_file(buffer, BUFFERSIZE);
		if (len == 0) break;
		move(size_y - 2, 0);
		clrtoeol();
		FILE* in = fopen(buffer, "rb");
		if (in == NULL) {
			attr_set(A_UNDERLINE, 4, NULL);
			mvaddstr(size_y - 2, 0, "Error: Could Not Open File \"");
			addstr(buffer);
			addch('\"');
			beep();
			continue;
		}
		maze src = load_maze(in);
		if (src.data == NULL) {
			attr_set(A_UNDERLINE, 4, NULL);
			mvaddstr(size_y - 2, 0, "Error: File \"");
			addstr(buffer);
			addstr("\" Not Properly Formated");
			fclose(in);
			beep();
			continue;
		}
		uiloc* path = malloc(sizeof(uiloc) * src.dim.r * src.dim.c * 2);
		if (path == NULL) goto Fatal;
		unsigned int path_count = scan_maze_d(src, path, (uiloc){0, 0}, (uiloc){src.dim.r - 1, src.dim.c - 1});
		for (int i = 1; i < size_y - 2; ++i) {
			move(i, 0);
			clrtoeol();
		}
		move(size_y >> 1, size_x >> 1);
		int res = visualize_maze(src, path, path_count);
		if (res < 0) goto Fatal;
		free(path);
		free_maze(src);
	}
	endwin();
	return 0;
	Fatal:
	endwin();
	fprintf(stderr, "Fatal Error: No Memory\n");
	return -1;
}
