#pragma once

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    index_accessor_type_text,
    index_accessor_type_binary
} index_accessor_type;

typedef struct {
    index_accessor_type type;
    const char* path;
} index_accessor;

void index_load_document(index_accessor accessor, const char* doc_name, char* doc_text);

#ifdef __cplusplus
}
#endif