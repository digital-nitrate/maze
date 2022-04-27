#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ncurses.h>

#include "maze.h"
#include "file_env.h"
#include "util_ui.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

#define BUFFERSIZE 64
#define HISTORY_DEPTH 1000

char const* const top_msg = "Welcome To Mad Maze Editor";

static inline void print_cell(maze m, unsigned int row, unsigned int col, int y, int x, int pair) {
	unsigned int const ind = row * m.dim.c + col;
	attr_set(A_NORMAL, 0, NULL);
	mvaddstr(y - m.dim.r + 1 + (row << 1), x - (m.dim.c << 1) + 1 + (col << 2), "    ");
	attr_set((m.data[ind].d) ? A_UNDERLINE : A_NORMAL, pair, NULL);
	mvprintw(y - m.dim.r + 1 + (row << 1), x - (m.dim.c << 1) + 1 + (col << 2), "%u", m.data[ind].len);	
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
	init_pair(5, COLOR_WHITE, COLOR_BLUE);
	attr_set(A_BOLD | A_UNDERLINE, 0, NULL);
	int size_y;
	int size_x;
	getmaxyx(stdscr, size_y, size_x);
	mvaddstr(0, (size_x - top_len) >> 1, top_msg);
	maze active = (maze){(uiloc){0, 0}, NULL};
	int c;
	unsigned int row = 0;
	unsigned int col = 0;
	int y = size_y >> 1;
	int x = size_x >> 1;
	attr_set(A_NORMAL, 0, NULL);
	mvprintw(1, x - 3, "%u", active.dim.r);
	mvaddch(1, x, 'x');
	mvprintw(1, x + 2, "%u", active.dim.c);
	curs_set(0);
	refresh();
	while ((c = getch()) != '\n') {
		switch (c) {
			case KEY_RESIZE:
				endwin();
				exit(-2);
			case KEY_DOWN:
				if (row + 1 >= active.dim.r) {
					beep();
				} else {
					print_cell(active, row, col, y, x, 0);
					++row;
					print_cell(active, row, col, y, x, 3);
				}
				break;
			case KEY_UP:
				if (row == 0) {
					beep();
				} else {
					print_cell(active, row, col, y, x, 0);
					--row;
					print_cell(active, row, col, y, x, 3);
				}
				break;
			case KEY_LEFT:
				if (col == 0) {
					beep();
				} else {
					print_cell(active, row, col, y, x, 0);
					--col;
					print_cell(active, row, col, y, x, 3);
				}
				break;
			case KEY_RIGHT:
				if (col + 1 >= active.dim.c) {
					beep();
				} else {
					print_cell(active, row, col, y, x, 0);
					++col;
					print_cell(active, row, col, y, x, 3);
				}
				break;
			case 'i':
				if (active.dim.r == 0 || active.dim.c == 0) {
					beep();
				} else {
					attr_set(A_NORMAL, 0, NULL);
					move(y - active.dim.r + 1 + (row << 1), x - (active.dim.c << 1) + 1 + (col << 2));
					int empty;
					unsigned int amt = get_uint(2, size_y - 2, size_y - 1, A_NORMAL, 0, A_UNDERLINE, 4, &empty);
					if (!empty) active.data[row * active.dim.c + col].len = amt;
					print_cell(active, row, col, y, x, 3);
				}
				break;
			case 's':
				if (active.dim.r == 0 || active.dim.c == 0) {
					beep();
				} else {
					move(size_y - 1, 0);
					clrtoeol();
					attr_set(A_NORMAL, 0, NULL);
					addstr("Enter File Name: ");
					size_t len = get_str(buffer, BUFFERSIZE, size_y - 2, size_y - 1, A_NORMAL, 0, A_UNDERLINE, 4);
					move(size_y - 1, 0);
					clrtoeol();
					if (len != 0) {
						FILE* out = fopen_tilde(buffer, "wb");
						if (out == NULL) {
							attr_set(A_UNDERLINE, 4, NULL);
							printw("Error: File \"%s\" Could Not Be Opened", buffer);
						} else {
							write_maze(out, active);
							fclose(out);
							addstr("Saved");
						}
					}
				}
				break;
			case 'd':
			{
				unsigned int old_row = active.dim.r;
				unsigned int old_col = active.dim.c;
				attr_set(A_NORMAL, 0, NULL);
				move(1, x - 3);
				int empty;
				unsigned amt = get_uint(2, size_y - 2, size_y - 1, A_NORMAL, 0, A_UNDERLINE, 4, &empty);
				if (!empty) active.dim.r = amt;
				mvprintw(1, x - 3, "%u", active.dim.r);
				move(1, x + 2);
				amt = get_uint(2, size_y - 2, size_y - 1, A_NORMAL, 0, A_UNDERLINE, 4, &empty);
				if (!empty) active.dim.c = amt;
				mvprintw(1, x + 2, "%u", active.dim.c);
				for (int i = 2; i < size_y - 1; ++i) {
					move(i, 0);
					clrtoeol();
				}
				if (active.dim.r == 0 || active.dim.c == 0) {
					free(active.data);
					active.data = NULL;
				} else {
					struct cell* data = malloc(sizeof(struct cell) * active.dim.r * active.dim.c);
					if (data == NULL) goto Fatal;
					struct cell* old = active.data;
					active.data = data;
					for (unsigned int i = 0; i < active.dim.r; ++i) {
						for (unsigned int j = 0; j < active.dim.c; ++j) {
							active.data[i * active.dim.c + j] = (i < old_row && j < old_col) ? old[i * old_col + j] : (struct cell){0, 0};
							print_cell(active, i, j, y, x, 0);
						}
					}
					free(old);
				}
				row = 0;
				col = 0;
				if (active.dim.r != 0 && active.dim.c != 0) {
					print_cell(active, 0, 0, y, x, 3);
				}
			}
				break;
			case 'c':
				if (active.dim.r == 0 || active.dim.c == 0) {
					beep();
				} else {
					uiloc start = (uiloc){row, col};
					move(size_y - 1, 0);
					clrtoeol();
					attr_set(A_NORMAL, 5, NULL);
					addstr("Select Goal");
					refresh();
					while ((c = getch()) != '\n' && c != 'c') {
						switch(c) {
							case KEY_RESIZE:
								endwin();
								exit(-2);
							case KEY_DOWN:
								if (row + 1 >= active.dim.r) {
									beep();
								} else {
									print_cell(active, row, col, y, x, 0);
									++row;
									print_cell(active, row, col, y, x, 3);
								}
								break;
							case KEY_UP:
								if (row == 0) {
									beep();
								} else {
									print_cell(active, row, col, y, x, 0);
									--row;
									print_cell(active, row, col, y, x, 3);
								}
								break;
							case KEY_LEFT:
								if (col == 0) {
									beep();
								} else {
									print_cell(active, row, col, y, x, 0);
									--col;
									print_cell(active, row, col, y, x, 3);
								}
								break;
							case KEY_RIGHT:
								if (col + 1 >= active.dim.c) {
									beep();
								} else {
									print_cell(active, row, col, y, x, 0);
									++col;
									print_cell(active, row, col, y, x, 3);
								}
								break;
						}
						print_cell(active, start.r, start.c, y, x, 3);
						refresh();
					}
					if (c == '\n') {
						move(size_y - 1, 0);
						clrtoeol();
						print_cell(active, start.r, start.c, y, x, 0);
						print_cell(active, row, col, y, x, 3);
						break;
					}
					uiloc goal = (uiloc){row, col};
					move(size_y - 1, 0);
					clrtoeol();
					attr_set(A_NORMAL, 5, NULL);
					addstr("Choose Search Type (d/b)");
					refresh();
					while ((c = getch()) != '\n' && c != 'd' && c != 'b') {
						if (c == KEY_RESIZE) {
							endwin();
							exit(-2);
						}
						beep();
					}
					if (c == '\n') {
						move(size_y - 1, 0);
						clrtoeol();
						print_cell(active, start.r, start.c, y, x, 0);
						break;
					}
					uiloc* path = malloc(sizeof(uiloc) * active.dim.r * active.dim.c * 2);
					unsigned char* visits = malloc(sizeof(unsigned char) * active.dim.r * active.dim.c);
					if (path == NULL || visits == NULL) goto Fatal;
					unsigned int path_count = (c == 'd') ? scan_maze_d(active, path, start, goal) : scan_maze_b(active, path, start, goal);
					uiloc* pcurr = path;
					uiloc* const pend = path + path_count;
					move(size_y - 1, 0);
					clrtoeol();
					if (path_count == 0) {
						beep();
						attr_set(A_NORMAL, 0, NULL);
						addstr("No Path Found");
						print_cell(active, start.r, start.c, y, x, 0);
					} else {
						attr_set(A_NORMAL, 5, NULL);
						addstr("Solution");
						unsigned char* const end = visits + (active.dim.r * active.dim.c);
						for (unsigned char* curr = visits; curr != end; ++curr) *curr = 0;
						print_cell(active, row, col, y, x, 0);
						++(visits[start.r * active.dim.c + start.c]);
						print_cell(active, start.r, start.c, y, x, 3);
						refresh();
						while ((c = getch()) != '\n') {
							switch(c) {
								case KEY_RESIZE:
									endwin();
									exit(-2);
								case KEY_RIGHT:
									if (pcurr + 1 == pend) {
										beep();
									} else {
										print_cell(active, pcurr->r, pcurr->c, y, x, visits[pcurr->r * active.dim.c + pcurr->c]);
										++pcurr;
										print_cell(active, pcurr->r, pcurr->c, y, x, 3);
										++(visits[pcurr->r * active.dim.c + pcurr->c]);
									}
									break;
								case KEY_LEFT:
									if (pcurr == path) {
										beep();
									} else {
										--(visits[pcurr->r * active.dim.c + pcurr->c]);
										print_cell(active, pcurr->r, pcurr->c, y, x, visits[pcurr->r * active.dim.c + pcurr->c]);
										--pcurr;
										print_cell(active, pcurr->r, pcurr->c, y, x, 3);
									}
									break;
							}
							refresh();
						}
						move(size_y - 1, 0);
						clrtoeol();
						for (unsigned int i = 0; i < active.dim.r; ++i) {
							for (unsigned int j = 0; j < active.dim.c; ++j) {
								print_cell(active, i, j, y, x, 0);
							}
						}
						row = pcurr->r;
						col = pcurr->c;
						print_cell(active, row, col, y, x, 3);
					}
					free(path);
					free(visits);
				}
				break;
			case 't':
				if (active.dim.r == 0 || active.dim.c == 0) {
					beep();
				} else {
					move(size_y - 1, 0);
					clrtoeol();
					attr_set(A_NORMAL, 5, NULL);
					addstr("Testing");
					uiloc* history = malloc(sizeof(uiloc) * HISTORY_DEPTH);
					if (history == NULL) goto Fatal;
					uiloc* curr = history;
					uiloc* const end = history + HISTORY_DEPTH;
					unsigned char type = 0;
					unsigned char histx = 0;
					unsigned char histy = 0;
					unsigned int row_adj;
					unsigned int col_adj;
					while ((c = getch()) != '\n') {
						unsigned int ind = row * active.dim.c + col;
						unsigned int len = active.data[ind].len;
						switch (c) {
							case KEY_RESIZE:
								endwin();
								exit(-2);
							case KEY_LEFT:
								if (type) {
									if (histy == 1) {
										histy = 0;
										if (col < len || row < len) {
											beep();
										} else {
											row_adj = row - len;
											col_adj = col - len;
											goto UpdateTest;
										}
									} else if (histy == 2) {
										histy = 0;
										if (col < len || row + len >= active.dim.r) {
											beep();
										} else {
											row_adj = row + len;
											col_adj = col - len;
											goto UpdateTest;
										}
									} else {
										histx = 1;
									}
								} else {
									if (col < len) {
										beep();
									} else {
										row_adj = row;
										col_adj = col - len;
										goto UpdateTest;
									}
								}
								break;
							case KEY_RIGHT:
								if (type) {
									if (histy == 1) {
										histy = 0;
										if (col + len >= active.dim.c || row < len) {
											beep();
										} else {
											row_adj = row - len;
											col_adj = col + len;
											goto UpdateTest;
										}
									} else if (histy == 2) {
										histy = 0;
										if (col + len >= active.dim.c || row + len >= active.dim.r) {
											beep();
										} else {
											row_adj = row + len;
											col_adj = col + len;
											goto UpdateTest;
										}
									} else {
										histx = 2;
									}
								} else {
									if (col + len >= active.dim.c) {
										beep();
									} else {
										row_adj = row;
										col_adj = col + len;
										goto UpdateTest;
									}
								}
								break;
							case KEY_UP:
								if (type) {
									if (histx == 1) {
										histx = 0;
										if (col < len || row < len) {
											beep();
										} else {
											row_adj = row - len;
											col_adj = col - len;
											goto UpdateTest;
										}
									} else if (histx == 2) {
										histx = 0;
										if (col + len >= active.dim.c || row < len) {
											beep();
										} else {
											row_adj = row - len;
											col_adj = col + len;
											goto UpdateTest;
										}
									} else {
										histy = 1;
									}
								} else {
									if (row < len) {
										beep();
									} else {
										row_adj = row - len;
										col_adj = col;
										goto UpdateTest;
									}
								}
								break;
							case KEY_DOWN:
								if (type) {
									if (histx == 1) {
										histx = 0;
										if (col < len || row + len >= active.dim.r) {
											beep();
										} else {
											row_adj = row + len;
											col_adj = col - len;
											goto UpdateTest;
										}
									} else if (histx == 2) {
										histx = 0;
										if (col + len >= active.dim.c || row + len >= active.dim.r) {
											beep();
										} else {
											row_adj = row + len;
											col_adj = col + len;
											goto UpdateTest;
										}
									} else {
										histy = 2;
									}
								} else {
									if (row + len >= active.dim.r) {
										beep();
									} else {
										row_adj = row + len;
										col_adj = col;
										goto UpdateTest;
									}
								}
								break;
							case KEY_BACKSPACE:
								histx = 0;
								histy = 0;
								if (curr == history) {
									beep();
								} else {
									print_cell(active, row, col, y, x, 0);
									--curr;
									row = curr->r;
									col = curr->c;
									print_cell(active, row, col, y, x, 3);
									if (active.data[ind].d) type ^= 1;
								}
								break;
						}
						continue;
						UpdateTest:
						if (curr == end) {
							beep();
							continue;
						}
						*curr = (uiloc){row, col};
						++curr;
						print_cell(active, row, col, y, x, 0);
						row = row_adj;
						col = col_adj;
						print_cell(active, row, col, y, x, 3);
						if (active.data[row * active.dim.c + col].d) type ^= 1;
						refresh();
					}
					free(history);
					move(size_y - 1, 0);
					clrtoeol();
				}
				break;
			case 'r':
			{
				move(size_y - 1, 0);
				clrtoeol();
				attr_set(A_NORMAL, 0, NULL);
				addstr("Enter File Name: ");
				size_t len = get_str(buffer, BUFFERSIZE, size_y - 2, size_y - 1, A_NORMAL, 0, A_UNDERLINE, 4);
				move(size_y - 1, 0);
				clrtoeol();
				if (len != 0) {
					FILE* out = fopen_tilde(buffer, "rb");
					if (out == NULL) {
						attr_set(A_UNDERLINE, 4, NULL);
						printw("Error: File \"%s\" Could Not Be Opened", buffer);
					} else {
						maze tmp = load_maze(out);
						fclose(out);
						if (tmp.data == NULL) {
							attr_set(A_UNDERLINE, 4, NULL);
							printw("Error: File \"%s\" Invalid Format", buffer);
						} else {
							free_maze(active);
							active = tmp;
							mvaddstr(size_y - 1, 0, "Read");
							move(1, 0);
							clrtoeol();
							mvprintw(1, x - 3, "%u", active.dim.r);
							mvaddch(1, x, 'x');
							mvprintw(1, x + 2, "%u", active.dim.c);
							row = 0;
							col = 0;
							for (int i = 2; i < size_y - 1; ++i) {
								move(i, 0);
								clrtoeol();
							}
							for (unsigned int i = 0; i < active.dim.r; ++i) {
								for (unsigned int j = 0; j < active.dim.c; ++j) {
									print_cell(active, i, j, y, x, 0);
								}
							}
							print_cell(active, 0, 0, y, x, 3);
						}
					}
				}
			}
				break;
			case 'm':
				if (active.dim.r == 0 || active.dim.c == 0) {
					beep();
				} else {
					active.data[row * active.dim.c + col].d ^= 1;
					print_cell(active, row, col, y, x, 3);
				}
				break;
		}
		curs_set(0);
		refresh();
	}
	free_maze(active);
	endwin();
	return 0;
	Fatal:
	endwin();
	fprintf(stderr, "Fatal Error: No Memory\n");
	return -1;
}
