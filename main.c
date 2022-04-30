/* Base maze project executable main code.
 */

#include <stdio.h>
#include <stdlib.h>

#include "maze.h" // Maze problem

#ifndef NULL
#define NULL ((void*)0)
#endif

#if defined(BRD) && defined(DEPTH)
#error "Cannot use both breadth-first and depth-first in the same basic executable"
#endif

#if !defined(BRD) && !defined(DEPTH)
#error "One of breath-first or depth-first must be specified"
#endif

int main(int argc, char** argv) {
	if (argc == 1) goto Error0; // Expect at least one argument
	FILE* in = fopen(argv[1], "rb");
	if (in == NULL) goto Error0; // 1st argument must be a valid file
	maze src = load_maze(in);
	fclose(in);
	if (src.data == NULL) goto Error0; // File must be valid format
	uiloc* path = malloc(sizeof(uiloc) * src.dim.r * src.dim.c * 2);
	if (path == NULL) goto Error1;
	#ifdef BRD
	unsigned int path_count = scan_maze_b(src, path, (uiloc){0, 0}, (uiloc){src.dim.r - 1, src.dim.c - 1});
	#endif
	#ifdef DEPTH
	unsigned int path_count = scan_maze_d(src, path, (uiloc){0, 0}, (uiloc){src.dim.r - 1, src.dim.c - 1});
	#endif
	if (path_count == 0) {
		fputs("No Path Found\n", stdout);
	} else {
		uiloc* end = path + path_count;
		fprintf(stdout, "(%u,%u)",path->r + 1, path->c + 1);
		for (uiloc* curr = path + 1; curr != end; ++curr) {
			fprintf(stdout, " (%u,%u)", curr->r + 1, curr->c + 1);
		}
		fputc('\n', stdout); // Display solution
	}
	free(path); // Release path
	free_maze(src); // Release maze data
	return 0;
	Error1:
	free_maze(src);
	Error0:
	return -1;
}
