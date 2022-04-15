SRC := main.c maze.c vis.c edit.c file_env.c util_ui.c
OBJ := d_main.o b_main.o maze.o vis.o main.o maze_d.o edit.o file_env.o util_ui.o
EXEC := d_solv b_solv visu d_debug b_debug editor

CFLAGS := -Wall -Wextra -Ofast -fno-asynchronous-unwind-tables -fno-unwind-tables -fno-ident
CFLAGS_EXE := -s -g0
LDFLAGS := -lncurses

.PHONY: all clean reset

all: $(EXEC)

editor: edit.o maze.o file_env.o util_ui.o
	$(CC) $(CFLAGS_EXE) $^ $(LDFLAGS) -o $@

d_debug: d_main.o maze_d.o
	$(CC) $(CFLAGS_EXE) $^ $(LDFLAGS) -o $@

b_debug: b_main.o maze_d.o
	$(CC) $(CFLAGS_EXE) $^ $(LDFLAGS) -o $@

visu: vis.o maze.o
	$(CC) $(CFLAGS_EXE) $^ $(LDFLAGS) -o $@

d_solv: d_main.o maze.o
	$(CC) $(CFLAGS_EXE) $^ $(LDFLAGS) -o $@

b_solv: b_main.o maze.o
	$(CC) $(CFLAGS_EXE) $^ $(LDFLAGS) -o $@

maze_d.o: maze.c
	$(CC) $(CFLAGS) -DDEBUG_LOG -c $< -o $@

d_main.o: main.c
	$(CC) $(CFLAGS) -DDEPTH -c $< -o $@

b_main.o: main.c
	$(CC) $(CFLAGS) -DBRD -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)

reset:
	$(RM) $(OBJ) $(EXEC)
