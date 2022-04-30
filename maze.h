#ifndef MAZE_H
#define MAZE_H 1

/* Maze Structure Header File
 *
 * Contains Maze Structure Definition, As Well
 * As Tools For Reading And Writing Mazes, 
 * Solving Mazes, And Memory Manegment For
 * The Generated Structure.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#undef __attribute__
#ifdef __GNUC__
#define __attribute__(args) __attribute__(args)
#else
#define __attribute__(args)
#endif

/* Structure to store cell location. It stores
 * row as an 8-bit unsigned integer as r and
 * column as an 8-bit unsigned integer as c.
 */
typedef struct {
	unsigned int r : 8, c : 8;
} uiloc;

/* Structure for cell information. It stores
 * jump length as an 8-bit unsigned integer as
 * len, and a 1-bit flag for if the cell switches
 * motion type.
 */
struct cell {
	unsigned int len : 8, d : 1;
};

/* Structure for a maze. It stores the maze dimensions
 * as a cell location in member dim. It shall store each
 * of the cell information as an array in member data.
 */
typedef struct {
	uiloc dim;
	struct cell* data;
} maze;

/* Loads a maze from file described by FILE* in.
 * Upon success, a valid maze structure is returned. On
 * failure, the maze.data field is set to NULL.
 */
extern maze load_maze(FILE* in) __attribute__((nonnull(1)));

/* Writes maze m to FILE* out in standard txt
 * format. Generally assumes success.
 */
extern void write_maze(FILE* out, maze m) __attribute__((nonnull(1)));

/* Frees data associated with a maze structure m.
 */
extern void free_maze(maze m);

/* Runs a depth-first search of maze m, from start to end
 * and saves the solution as an array at path. It shall return
 * the length of this generated array. Upon success, the length
 * shall be non-zero. On failure, the returned length shall be
 * zero.
 */
extern unsigned int scan_maze_d(maze m, uiloc* path, uiloc start, uiloc end) __attribute__((nonnull(2)));

/* Runs a breadth-first search of maze m, from start to end
 * and saves the solution as an array at path. It shall return
 * the length of this generated array. Upon success, the length
 * shall be non-zero. On failure, the returned length shall be
 * zero.
 */
extern unsigned int scan_maze_b(maze m, uiloc* path, uiloc start, uiloc end) __attribute__((nonnull(2)));

#ifdef __cplusplus
}
#endif

#endif
