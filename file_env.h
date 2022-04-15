#ifndef FILE_ENV_H
#define FILE_ENV_H 1

#include <stddef.h>
#include <stdio.h>

FILE* fopen_tilde(char const* restrict filename, const char* restrict mode);
size_t tilde_exp(char const* name, char* buffer, size_t buffer_len);
size_t match_find(char const* base_path, char* cbuffer, size_t cbuffer_len, char* mbuffer, size_t mbuffer_len, size_t* clen, size_t* mlen);
size_t tilde_find(char const* base_name, char* cbuffer, size_t cbuffer_len, char* mbuffer, size_t mbuffer_len, size_t* clen, size_t* mlen);

#endif
