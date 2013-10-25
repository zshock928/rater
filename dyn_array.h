#ifndef __dyn_array_h__
#define __dyn_array_h__

#include <stdlib.h>

// add as much as possible of the given string to the result, and return the
// number of characters written
size_t add_str(const char *addend, char *result, size_t maxlen);

// same as above, but with format strings
// except if the string is too long, then it just won't be written, since
// determining legitimate substrings is messy with format specifiers
// XXX addend should really be at the end, but oh well
size_t add_strf(const char *addend, char *result, size_t maxlen, ...);

size_t char_to_str(char *c, char *result, size_t maxlen);

static const size_t DYNAMIC_ARRAY_INIT_CAPACITY = 4;

// macro for defining a dynamic array with a custom name and type
// size_t STRFUNC(TYPE *item, char *result, size_t maxlen)
#define DYNAMIC_ARRAY_DECL(NAME, LC_NAME, TYPE) \
    typedef struct NAME {                                               \
        size_t count;                                                   \
        size_t capacity;                                                \
        TYPE *data;                                                     \
    } NAME;                                                             \
    void LC_NAME ## _init(NAME * arr);                                  \
    size_t LC_NAME ## _add(NAME *arr, const TYPE *item);                \
    void LC_NAME ## _free(NAME *arr);                                   \
    size_t LC_NAME ## _to_str(NAME *arr, char *result, size_t maxlen);  \
    typedef size_t (*NAME ## StrFuncPtr)(TYPE *, char *, size_t);       \
    typedef size_t (*NAME ## FreeFuncPtr)(TYPE *);

#define DYNAMIC_ARRAY_DEF(NAME, LC_NAME, TYPE, STRFUNC, START, SEP, END, FREEFUNC) \
    void LC_NAME ## _init(NAME * arr) {                                 \
        arr->count = 0;                                                 \
        arr->capacity = DYNAMIC_ARRAY_INIT_CAPACITY;                    \
        arr->data = (TYPE *)malloc(arr->capacity * sizeof(TYPE));       \
        assert(arr->data);                                              \
    }                                                                   \
    size_t LC_NAME ## _add(NAME *arr, const TYPE *item) {               \
        size_t index = arr->count++;                                    \
        if (arr->count > arr->capacity) {                               \
            arr->capacity *= 2;                                         \
            assert(arr->capacity > arr->count);                         \
            arr->data = (TYPE *)realloc(arr->data, arr->capacity * sizeof(TYPE)); \
            assert(arr->data);                                          \
        }                                                               \
        memcpy(&arr->data[index], item, sizeof(TYPE));                  \
        return index;                                                   \
    }                                                                   \
    void LC_NAME ## _free(NAME *arr) {                                  \
        if (FREEFUNC != NULL) {                                         \
            TYPE *data_curr = arr->data;                                \
            while (data_curr != arr->data + arr->count) {               \
                FREEFUNC(data_curr++);                                  \
            }                                                           \
        }                                                               \
        free(arr->data);                                                \
        arr->data = NULL;                                               \
        arr->count = arr->capacity = 0;                                 \
    }                                                                   \
    size_t LC_NAME ## _to_str(NAME *arr, char *result, size_t maxlen) {   \
        if (maxlen == 0) return 0;                                      \
        char *str_curr = result;                                        \
        char *str_end = result + maxlen;                                \
        TYPE *arr_curr = arr->data;                                     \
        TYPE *arr_end = arr->data + arr->count;                         \
        str_curr += add_str(START, str_curr, str_end - str_curr);       \
        while (STRFUNC != NULL && str_curr != str_end && arr_curr != arr_end) { \
            str_curr += STRFUNC(arr_curr++, str_curr, str_end - str_curr); \
            if (arr_curr != arr_end) {                                  \
                str_curr += add_str(SEP, str_curr, str_end - str_curr); \
            }                                                           \
        }                                                               \
        str_curr += add_str(END, str_curr, str_end - str_curr);         \
        return str_curr - result;                                       \
    }

// define a "String" struct which can be dynamically added to
DYNAMIC_ARRAY_DECL(String, string, char);

String char_arr_to_string(const char *char_arr);

#endif  // __dyn_array_h__
