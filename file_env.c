#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>

#include "file_env.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

#define EXPANPATH 4096

FILE* fopen_tilde(char const* restrict filename, char const* restrict mode) {
	if (*filename == '~') {
		char const* file_end = filename + 1;
		while (*file_end !=  '\0' && *file_end != '/') ++file_end;
		char const* const file_tilde = file_end;
		while (*file_end != '\0') ++file_end;
		char expanded[EXPANPATH + (file_end - file_tilde)];
		char* exp_curr = expanded;
		char const* file_curr = filename + 1;
		while (file_curr != file_tilde) {
			*exp_curr = *file_curr;
			++exp_curr;
			++file_curr;
		}
		*exp_curr = '\0';
		size_t ext_len = tilde_exp(expanded, expanded, EXPANPATH);
		exp_curr = expanded + ext_len;
		while (file_curr != file_end) {
			*exp_curr = *file_curr;
			++exp_curr;
			++file_curr;
		}
		*exp_curr = '\0';
		return fopen(expanded, mode);
	} else {
		return fopen(filename, mode);
	}
}

size_t tilde_exp(char const* name, char* buffer, size_t buffer_len) {
	if (buffer_len == 0) return 0;
	char* val;
	if (*name == '\0') {
		val = getenv("HOME");
	} else {
		struct passwd* user = getpwnam(name);
		if (user == NULL) return 0;
		val = user->pw_dir;
	}
	if (val == NULL) return 0;
	char const* end = buffer + buffer_len - 1;
	char* curr = buffer;
	while (curr != end && *val != '\0') {
		*curr = *val;
		++curr;
		++val;
	}
	*curr = '\0';
	return curr - buffer;
}

size_t match_find(char const* base_path, char* cbuffer, size_t cbuffer_len, char* mbuffer, size_t mbuffer_len, size_t* clen, size_t* mlen) {
	char* dir_loc = cbuffer;
	char* const cbuf_end = cbuffer + cbuffer_len - 1;
	char* cbuf_curr = cbuffer;
	for (char const* base_curr = base_path; *base_curr != '\0' && cbuf_curr != cbuf_end; ++base_curr) {
		*cbuf_curr = *base_curr;
		++cbuf_curr;
		if (*base_curr == '/') dir_loc = cbuf_curr;
	}
	char* const cbuf_split = cbuf_curr;
	DIR* d;
	if (dir_loc == cbuffer)  {
		d = opendir(".");
	} else {
		char save = *dir_loc;
		*dir_loc = '\0';
		d = opendir(cbuffer);
		*dir_loc = save;
	}
	size_t matches = 0;
	char* const mbuf_end = mbuffer + mbuffer_len - 1;
	char* mbuf_curr = mbuffer;
	if (d == NULL) goto Finish;
	unsigned char type = DT_UNKNOWN;
	for (struct dirent* ent = readdir(d); ent != NULL; ent = readdir(d)) {
		char* ent_curr = ent->d_name;
		char* ent_cbuf = dir_loc;
		while (ent_cbuf < cbuf_split) {
			if (*ent_curr != *ent_cbuf) break;
			++ent_curr;
			++ent_cbuf;
		}
		if (ent_cbuf == cbuf_split && *ent_curr != '.') {
			if (matches == 0) {
				while (*ent_curr != '\0' && ent_cbuf != cbuf_end) {
					*ent_cbuf = *ent_curr;
					++ent_curr;
					++ent_cbuf;
				}
				type = ent->d_type;
			} else {
				while (ent_cbuf != cbuf_curr) {
					if (*ent_curr != *ent_cbuf) break;
					++ent_curr;
					++ent_cbuf;
				}
			}
			while (*ent_curr != '\0') {
				++ent_curr;
			}
			size_t len = ent_curr - ent->d_name;
			size_t space = mbuf_end - mbuf_curr;
			if (len < space) {
				char* const ent_end = ent_curr;
				ent_curr = ent->d_name;
				while (ent_curr != ent_end) {
					*mbuf_curr = *ent_curr;
					++mbuf_curr;
					++ent_curr;
				}
				*mbuf_curr = ' ';
				++mbuf_curr;
			}
			cbuf_curr = ent_cbuf;
			++matches;
		}
	}
	closedir(d);
	if (matches == 1 && type == DT_DIR && cbuf_curr != cbuf_end) {
		*cbuf_curr = '/';
		++cbuf_curr;
	}
	Finish:
	*cbuf_curr = '\0';
	*mbuf_curr = '\0';
	if (clen != NULL) *clen = cbuf_curr - cbuffer;
	if (mlen != NULL) *mlen = mbuf_curr - mbuffer;
	return matches;
}

size_t tilde_find(char const* base_name, char* cbuffer, size_t cbuffer_len, char* mbuffer, size_t mbuffer_len, size_t* clen, size_t* mlen) {
	char* const cbuf_end = cbuffer + cbuffer_len - 1;
	char* cbuf_curr = cbuffer;
	for (char const* base_curr = base_name; *base_curr != '\0' && cbuf_curr != cbuf_end; ++base_curr) {
		*cbuf_curr = *base_curr;
		++cbuf_curr;
	}
	char* const cbuf_split = cbuf_curr;
	size_t matches = 0;
	char* const mbuf_end = mbuffer + mbuffer_len - 1;
	char* mbuf_curr = mbuffer;
	for (struct passwd* ent = getpwent(); ent != NULL; ent = getpwent()) {
		char* ent_curr = ent->pw_name;
		char* ent_cbuf = cbuffer;
		while (ent_cbuf < cbuf_split) {
			if (*ent_curr != *ent_cbuf) break;
			++ent_curr;
			++ent_cbuf;
		}
		if (ent_cbuf == cbuf_split) {
			if (matches == 0) {
				while (*ent_curr != '\0' && ent_cbuf != cbuf_end) {
					*ent_cbuf = *ent_curr;
					++ent_curr;
					++ent_cbuf;
				}
			} else {
				while (ent_cbuf != cbuf_curr) {
					if (*ent_curr != *ent_cbuf) break;
					++ent_curr;
					++ent_cbuf;
				}
			}
			while (*ent_curr != '\0') {
				++ent_curr;
			}
			size_t len = ent_curr - ent->pw_name;
			size_t space = mbuf_end - mbuf_curr;
			if (len < space) {
				char* const ent_end = ent_curr;
				ent_curr = ent->pw_name;
				while (ent_curr != ent_end) {
					*mbuf_curr = *ent_curr;
					++mbuf_curr;
					++ent_curr;
				}
				*mbuf_curr = ' ';
				++mbuf_curr;
			}
			cbuf_curr = ent_cbuf;
			++matches;
		}
	}
	endpwent();
	if (matches == 1 && cbuf_curr != cbuf_end) {
		*cbuf_curr = '/';
		++cbuf_curr;
	}
	*cbuf_curr = '\0';
	*mbuf_curr = '\0';
	if (clen != NULL) *clen = cbuf_curr - cbuffer;
	if (mlen != NULL) *mlen = mbuf_curr - mbuffer;
	return matches;
}
