#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <malloc.h>
#include <mcheck.h>

#include "dyn_array.h"

size_t add_str(const char *addend, char *result, size_t maxlen)
{
    assert(addend && result);
    if (maxlen == 0 || *addend == '\0') return 0;
    memcpy(result, addend, maxlen);
    return strlen(result);
}

size_t add_strf(const char *addend, char *result, size_t maxlen, ...)
{
    assert(addend && result);
    if (maxlen == 0 || *addend == '\0') return 0;
    va_list args1, args2;
    va_start(args1, maxlen);
    // determine what the length of the generated string would be
    size_t len = vsnprintf(NULL, 0, addend, args1);
    va_end(args1);
    if (len > maxlen) return 0;

    va_start(args2, maxlen);
    // take trailing null into account
    vsnprintf(result, len + 1, addend, args2);
    va_end(args2);
    return len;
}

size_t char_to_str(char *c, char *result, size_t maxlen)
{
    if (maxlen == 0) return 0;
    *result = *c;
    return 1;
}

DYNAMIC_ARRAY_DEF(String, string, char, char_to_str, "", "", "", ((StringFreeFuncPtr)NULL));

String char_arr_to_string(const char *char_arr)
{
    String str;
    str.count = str.capacity = strlen(char_arr);
    str.data = (char *)malloc(str.count * sizeof(char));
    assert(str.data);
    memcpy(str.data, char_arr, str.count * sizeof(char));
    return str;
}

/*
typedef struct String {
    size_t count;
    size_t capacity;
    char *data;
} String;
void string_init(String * arr) {
    arr = (String *)malloc(sizeof(String));
    assert(arr);
    arr->count = 0;
    arr->capacity = DYNAMIC_ARRAY_INIT_CAPACITY;
    arr->data = (char *)malloc(arr->capacity * sizeof(char));
    assert(arr->data);
}
size_t string_add(String *arr, const char *item) {
    size_t index = arr->count++;
    if (arr->count > arr->capacity) {
        arr->capacity *= 2;
        assert(arr->capacity > arr->count);
        arr->data = (char *)realloc(arr->data, arr->capacity * sizeof(char));
        assert(arr->data);
    }
    memcpy(&arr->data[index], item, sizeof(char));
    return index;
}
void string_free(String *arr) {
    free(arr->data);
    arr->data = ((void *)0);
    arr->count = arr->capacity = 0;
}
void string_to_str(String *arr, char *result, size_t maxlen) {
    char *str_curr = result;
    char *str_end = result + maxlen;
    char *arr_curr = arr->data;
    char *arr_end = arr->data + arr->count;
    for (;
         arr_curr != arr_end;
         ++arr_curr) {
        char_to_str(arr_curr, str_curr, str_end - str_curr);
        assert(strlen(str_curr) <= str_end - str_curr);
        if (strlen(str_curr) == str_end - str_curr) {
            return;
        }
        str_curr += strlen(str_curr);
    }
}
*/

/*
int main(int argc, char *argv[])
{
    int mcheck_ok = mcheck(NULL);
    assert(mcheck_ok == 0);

    char result[101] = {0};
    String *str = char_arr_to_string(argv[0]);
    char c = 'd';
    string_add(str, &c);
    string_add(str, &c);
    string_add(str, &c);
    string_to_str(str, result, 100);
    printf("%s\n", result);
    string_free(str);
    free(str);

    struct mallinfo mi = mallinfo();
    printf("malloc info:\n"
           "arena: %d\n"
           "ordblks: %d\n"
           "smblks: %d\n"
           "hblks: %d\n"
           "hblkhd: %d\n"
           "usmblks: %d\n"
           "fsmblks: %d\n"
           "uordblks: %d\n"
           "fordblks: %d\n"
           "keepcost: %d\n",
           mi.arena, mi.ordblks, mi.smblks, mi.hblks, mi.hblkhd, mi.usmblks,
           mi.fsmblks, mi.uordblks, mi.fordblks, mi.keepcost);
    return 0;
}
*/
