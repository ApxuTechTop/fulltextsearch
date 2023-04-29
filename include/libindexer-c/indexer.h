#pragma once

#include <libparser-c/parser.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* Index;

Index create_index(Configuration* config);
void free_index(Index index);
void index_add_document(Index index, int doc_id, const char* doc_text);
void text_index_write(Index index, const char* path);

#ifdef __cplusplus
}
#endif