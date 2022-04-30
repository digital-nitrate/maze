#ifndef FILE_ENV_H
#define FILE_ENV_H 1

/* Environment Communtication Header File
 *
 * Contains Utility Functions For File Name
 * Expansion, Such As Locating Matches, And
 * Expanding Tidles.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdio.h>

#undef __attribute__
#ifdef __GNUC__
#define __attribute__(args) __attribute__(args)
#else
#define __attribute__(args)
#endif

/* Opens a file where the pathname can
 * contain tildes that shall be expanded
 * for user specific home directories.
 */
FILE* fopen_tilde(char const* restrict filename, const char* restrict mode) __attribute__((nonnull(1), nonnull(2)));

/* Expands the given name into a user
 * home directory path that is written
 * to in a buffer of known size as a null
 * terminated string. It shall return
 * the size of generated pathname.
 */
size_t tilde_exp(char const* name, char* buffer, size_t buffer_len) __attribute__((nonnull(1), nonnull(2)));

/* Locates files that match somewhat with
 * the base path entered. It shall write
 * the longest common match to cbuffer, 
 * and shall write the set of matches as space
 * separated entries into mbuffer. One can
 * optionally pass pointers to clen and mlen
 * to get the lengths of these generated strings.
 * It shall return the number of matches found.
 */
size_t match_find(char const* base_path, char* cbuffer, size_t cbuffer_len, char* mbuffer, size_t mbuffer_len, size_t* clen, size_t* mlen) __attribute__((nonnull(1), nonnull(2), nonnull(4)));

/* Locates user names that match with the
 * base of the name entered. It shall write
 * the longest common match as a null terminated
 * string to cbuffer. It shall write the set of
 * matches as spaces separated entries into mbuffer.
 * One can optionally pass pointers to clen and mlen
 * to get the lengths of these generated strings.
 * It shall return the number matches found.
 */
size_t tilde_find(char const* base_name, char* cbuffer, size_t cbuffer_len, char* mbuffer, size_t mbuffer_len, size_t* clen, size_t* mlen) __attribute__((nonnull(1), nonnull(2), nonnull(4)));

#ifdef __cplusplus
}
#endif

#endif
