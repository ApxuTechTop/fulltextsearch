#pragma once



#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int ngrammin;
    int ngrammax;
    size_t count;
    const char** stopwords;
} Configuration;

typedef struct {
    size_t pos;
    char* ngram;
} Term;


#ifdef __cplusplus
}
#endif