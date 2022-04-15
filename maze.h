#ifndef MAZE_H
#define MAZE_H 1

#include <stdio.h>

typedef struct {
	unsigned int r : 8, c : 8;
} uiloc;

struct cell {
	unsigned int len : 8, d : 1;
};

typedef struct {
	uiloc dim;
	struct cell* data;
} maze;

extern maze load_maze(FILE* in);
extern void write_maze(FILE* out, maze m);
extern void free_maze(maze m);
extern unsigned int scan_maze_d(maze m, uiloc* path, uiloc start, uiloc end);
extern unsigned int scan_maze_b(maze m, uiloc* path, uiloc start, uiloc end);

#endif
