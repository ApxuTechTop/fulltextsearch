#pragma once

#include <libaccessor-c/accessor.h>
#include <libparser-c/parser.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double score;
    int document_id;
} Result;

typedef struct {
    void* vector_ptr;
    void* data;
    size_t count;
} vector_wrap;


vector_wrap accessor_search(index_accessor accessor, const char* query, Configuration* config);


#ifdef __cplusplus
}
#endif