#include <stdio.h>
#include <stdlib.h>

#include "maze.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

struct depth_cell {
	unsigned int len : 8, d : 1, v : 2;
};

struct recu_cell {
	unsigned int len : 8, d : 1, v : 2;
	uiloc ploc[2];
};

struct node {
	struct node* next;
	uiloc loc;
	unsigned int dist;
	unsigned char type;
};

maze load_maze(FILE* in) {
	int res;
	maze out;
	unsigned int row;
	unsigned int col;
	res = fscanf(in, "%u%u", &row, &col);
	out.dim = (uiloc){row, col};
	if (res != 2 || out.dim.r == 0 || out.dim.c == 0) goto Error0;
	struct cell* data = malloc(sizeof(struct cell) * out.dim.r * out.dim.c);
	if (data == NULL) goto Error0;
	struct cell* const end = data + (out.dim.r * out.dim.c);
	for (struct cell* curr = data; curr != end; ++curr) {
		int dir;
		res = fscanf(in, "%u", &dir);
		if (res != 1) goto Error1;
		*curr = (dir < 0) ? (struct cell){-dir, 1} : (struct cell){dir, 0};
	}
	out.data = data;
	return out;
	Error1:
	free(data);
	Error0:
	return (maze){(uiloc){0, 0}, NULL};
}

void write_maze(FILE* out, maze m) {
	fprintf(out, "%u %u\n", m.dim.r, m.dim.c);
	struct cell* curr = m.data;
	for (unsigned int i = 0; i < m.dim.r; ++i) {
		fprintf(out, "%d", curr->d ? -(curr->len) : curr->len);
		++curr;
		for (unsigned int j = 1; j < m.dim.c; ++j) {
			fprintf(out, " %d", curr->d ? -(curr->len) : curr->len);
			++curr;
		}
		fputc('\n', out);
	}
}

void free_maze(maze m) {
	free(m.data);
}

unsigned int scan_maze_d(maze m, uiloc* path, uiloc start, uiloc end) {
	struct depth_cell* const data = malloc(sizeof(struct depth_cell) * m.dim.r * m.dim.c);
	if (data == NULL) return 0;
	for (size_t i = 0; i < m.dim.r * m.dim.c; ++i) {
		data[i] = (struct depth_cell){m.data[i].len, m.data[i].d, 0};
	}
	uiloc const dim = m.dim;
	uiloc loc_c = start;
	unsigned char type_c = 0;
	data[start.r * dim.c + start.c].v |= 1;
	*path = start;
	uiloc* curr = path;
	struct depth_cell* cell_c = data + (start.r * dim.c + start.c);
	#ifdef DEBUG_LOG
	fputs("BEG\n", stderr);
	#endif
	while (loc_c.r != end.r || loc_c.c != end.c) {
		#ifdef DEBUG_LOG
		fprintf(stderr, "%tu - (%u, %u) - %s: Scan ... ", curr - path, loc_c.r + 1, loc_c.c + 1, (type_c) ? "diagonal" : "adjacent");
		#endif
		unsigned int len = cell_c->len;
		uiloc loc_n;
		struct depth_cell* cell_n;
		unsigned char type_n;
		if (type_c) {
			if (loc_c.r + len < dim.r && loc_c.c + len < dim.c) {
				loc_n = (uiloc){loc_c.r + len, loc_c.c + len};
				cell_n = data + (loc_n.r * dim.c + loc_n.c);
				type_n = type_c ^ cell_n->d;
				#ifdef DEBUG_LOG
				fputs("SE ", stderr);
				#endif
				if (!(cell_n->v & (1 << type_n))) goto SetCell;
			}
			if (len <= loc_c.r && loc_c.c + len < dim.c) {
				loc_n = (uiloc){loc_c.r - len, loc_c.c + len};
				cell_n = data + (loc_n.r * dim.c + loc_n.c);
				type_n = type_c ^ cell_n->d;
				#ifdef DEBUG_LOG
				fputs("NE ", stderr);
				#endif
				if (!(cell_n->v & (1 << type_n))) goto SetCell;
			}
			if (loc_c.r + len < dim.r && len <= loc_c.c) {
				loc_n = (uiloc){loc_c.r + len, loc_c.c - len};
				cell_n = data + (loc_n.r * dim.c + loc_n.c);
				type_n = type_c ^ cell_n->d;
				#ifdef DEBUG_LOG
				fputs("SW ", stderr);
				#endif
				if (!(cell_n->v & (1 << type_n))) goto SetCell;
			}
			if (len <= loc_c.r && len <= loc_c.c) {
				loc_n = (uiloc){loc_c.r - len, loc_c.c - len};
				cell_n = data + (loc_n.r * dim.c + loc_n.c);
				type_n = type_c ^ cell_n->d;
				#ifdef DEBUG_LOG
				fputs("NW ", stderr);
				#endif
				if (!(cell_n->v & (1 << type_n))) goto SetCell;
			}
		} else {
			if (loc_c.r + len < dim.r) {
				loc_n = (uiloc){loc_c.r + len, loc_c.c};
				cell_n = data + (loc_n.r * dim.c + loc_n.c);
				type_n = type_c ^ cell_n->d;
				#ifdef DEBUG_LOG
				fputs("S ", stderr);
				#endif
				if (!(cell_n->v & (1 << type_n))) goto SetCell;
			}
			if (loc_c.c + len < dim.c) {
				loc_n = (uiloc){loc_c.r, loc_c.c + len};
				cell_n = data + (loc_n.r * dim.c + loc_n.c);
				type_n = type_c ^ cell_n->d;
				#ifdef DEBUG_LOG
				fputs("E ", stderr);
				#endif
				if (!(cell_n->v & (1 << type_n))) goto SetCell;
			}
			if (len <= loc_c.r) {
				loc_n = (uiloc){loc_c.r - len, loc_c.c};
				cell_n = data + (loc_n.r * dim.c + loc_n.c);
				type_n = type_c ^ cell_n->d;
				#ifdef DEBUG_LOG
				fputs("N ", stderr);
				#endif
				if (!(cell_n->v & (1 << type_n))) goto SetCell;
			}
			if (len <= loc_c.c) {
				loc_n = (uiloc){loc_c.r, loc_c.c - len};
				cell_n = data + (loc_n.r * dim.c + loc_n.c);
				type_n = type_c ^ cell_n->d;
				#ifdef DEBUG_LOG
				fputs("W ", stderr);
				#endif
				if (!(cell_n->v & (1 << type_n))) goto SetCell;
			}	
		}
		#ifdef DEBUG_LOG
		fputs("... BACK\n", stderr);
		#endif
		if (curr == path) break;
		--curr;
		loc_c = *curr;
		type_c ^= cell_c->d;
		cell_c = data + (loc_c.r * dim.c + loc_c.c);
		continue;
		SetCell:
		++curr;
		cell_c = cell_n;
		loc_c = loc_n;
		type_c = type_n;
		*curr = loc_n;
		cell_n->v |= (1 << type_n);
		#ifdef DEBUG_LOG
		fputs("... FORWARD\n", stderr);
		#endif
	}
	#ifdef DEBUG_LOG
	fputs("FIN\n", stderr);
	#endif
	free(data);
	if (curr == path) {
		return (dim.r == 1 && dim.c == 1) ? 1 : 0;
	}
	return (curr - path) + 1;
}

static inline int add_cell(struct recu_cell* data, uiloc dim, struct node** q, uiloc loc_c, unsigned char type_c, unsigned int dist_c, uiloc loc_n) {
	struct recu_cell* cell_n = data + (loc_n.r * dim.c + loc_n.c);
	unsigned char type_n = type_c ^ (cell_n->d);
	if (!(cell_n->v & (1 << type_n))) {
		#ifdef DEBUG_LOG
		fputs("ADD ", stderr);
		#endif
		struct node* new_node = malloc(sizeof(struct node));
		if (new_node == NULL) return -1;
		cell_n->v |= (1 << type_n);
		cell_n->ploc[type_c] = loc_c;
		unsigned int dist_n = dist_c + 1;
		*new_node = (struct node){NULL, loc_n, dist_n, type_n};
		(*q)->next = new_node;
		*q = new_node;
	}
	return 0;
}

unsigned int scan_maze_b(maze m, uiloc* path, uiloc start, uiloc end) {
	struct recu_cell* const data = malloc(sizeof(struct recu_cell) * m.dim.r * m.dim.c);
	if (data == NULL) goto Error0;
	for (size_t i = 0; i < m.dim.r * m.dim.c; ++i) {
		data[i] = (struct recu_cell){m.data[i].len, m.data[i].d, 0, (uiloc){0, 0}, (uiloc){0, 0}};
	}
	uiloc const dim = m.dim;
	struct node* tmp = malloc(sizeof(struct node));
	if (tmp == NULL) goto Error1;
	*tmp = (struct node){NULL, start, 0, 0};
	struct node* queue_first = tmp;
	struct node* queue_last = tmp;
	data[start.r * dim.c + start.c].v |= 1;
	#ifdef DEBUG_LOG
	fputs("BEG\n", stderr);
	#endif
	while (queue_first != NULL) {
		uiloc loc_c = queue_first->loc;
		if (loc_c.r == end.r && loc_c.c == end.c) break;
		unsigned char type_c = queue_first->type;
		unsigned int dist_c = queue_first->dist;
		unsigned int len = data[loc_c.r * dim.c + loc_c.c].len;
		#ifdef DEBUG_LOG
		fprintf(stderr, "PULL (%u, %u) - %s: Scan ... ", loc_c.r + 1, loc_c.c + 1, (type_c) ? "diagonal" : "adjacent");
		#endif
		if (type_c) {
			if (loc_c.r + len < dim.r && loc_c.c + len < dim.c) {
				#ifdef DEBUG_LOG
				fputs("SE ", stderr);
				#endif
				uiloc loc_n = (uiloc){loc_c.r + len, loc_c.c + len};
				int chk = add_cell(data, dim, &queue_last, loc_c, type_c, dist_c, loc_n);
				if (chk < 0) goto Error2;
			}
			if (len <= loc_c.r && loc_c.c + len < dim.c) {
				#ifdef DEBUG_LOG
				fputs("NE ", stderr);
				#endif
				uiloc loc_n = (uiloc){loc_c.r - len, loc_c.c + len};
				int chk = add_cell(data, dim, &queue_last, loc_c, type_c, dist_c, loc_n);
				if (chk < 0) goto Error2;
			}
			if (loc_c.r + len < dim.r && len <= loc_c.c) {
				#ifdef DEBUG_LOG
				fputs("SW ", stderr);
				#endif
				uiloc loc_n = (uiloc){loc_c.r + len, loc_c.c - len};
				int chk = add_cell(data, dim, &queue_last, loc_c, type_c, dist_c, loc_n);
				if (chk < 0) goto Error2;
			}
			if (len <= loc_c.r && len <= loc_c.c) {
				#ifdef DEBUG_LOG
				fputs("NW ", stderr);
				#endif
				uiloc loc_n = (uiloc){loc_c.r - len, loc_c.c - len};
				int chk = add_cell(data, dim, &queue_last, loc_c, type_c, dist_c, loc_n);
				if (chk < 0) goto Error2;
			}
		} else {
			if (loc_c.r + len < dim.r) {
				#ifdef DEBUG_LOG
				fputs("S ", stderr);
				#endif
				uiloc loc_n = (uiloc){loc_c.r + len, loc_c.c};
				int chk = add_cell(data, dim, &queue_last, loc_c, type_c, dist_c, loc_n);
				if (chk < 0) goto Error2;
			}
			if (loc_c.c + len < dim.c) {
				#ifdef DEBUG_LOG
				fputs("E ", stderr);
				#endif
				uiloc loc_n = (uiloc){loc_c.r, loc_c.c + len};
				int chk = add_cell(data, dim, &queue_last, loc_c, type_c, dist_c, loc_n);
				if (chk < 0) goto Error2;
			}
			if (len <= loc_c.r) {
				#ifdef DEBUG_LOG
				fputs("N ", stderr);
				#endif
				uiloc loc_n = (uiloc){loc_c.r - len, loc_c.c};
				int chk = add_cell(data, dim, &queue_last, loc_c, type_c, dist_c, loc_n);
				if (chk < 0) goto Error2;
			}
			if (len <= loc_c.c) {
				#ifdef DEBUG_LOG
				fputs("W ", stderr);
				#endif
				uiloc loc_n = (uiloc){loc_c.r, loc_c.c - len};
				int chk = add_cell(data, dim, &queue_last, loc_c, type_c, dist_c, loc_n);
				if (chk < 0) goto Error2;
			}
		}
		#ifdef DEBUG_LOG
		fputs("... REMOVE\n", stderr);
		#endif
		struct node* mid = queue_first;
		queue_first = queue_first->next;
		free(mid);
	}
	#ifdef DEBUG_LOG
	fputs("FIN\n", stderr);
	#endif
	if (queue_first == NULL) return 0;	
	unsigned int path_count = queue_first->dist + 1;
	uiloc* path_curr = path + queue_first->dist;
	unsigned char type_c = queue_first->type;
	uiloc loc_c = queue_first->loc;
	for (unsigned int i = 0; i < path_count; ++i) {
		*path_curr = loc_c;
		--path_curr;
		type_c ^= data[loc_c.r * dim.c + loc_c.c].d;
		loc_c = data[loc_c.r * dim.c + loc_c.c].ploc[type_c];
	}
	while (queue_first != NULL) {
		struct node* tmp = queue_first;
		queue_first = queue_first->next;
		free(tmp);
	}
	free(data);
	return path_count;
	Error2:
	while (queue_first != NULL) {
		struct node* tmp = queue_first;
		queue_first = queue_first->next;
		free(tmp);
	}
	Error1:
	free(data);
	Error0:
	return 0;
}
