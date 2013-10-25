#include "song.h"

size_t song_to_str(Song *s, char *result, size_t maxlen)
{
    char *curr = result;
    char *end = result + maxlen;
    curr += string_to_str(&s->name, curr, end - curr);
    curr += add_str(" (", curr, end - curr);
    curr += string_to_str(&s->filepath, curr, end - curr);
    curr += add_str(")", curr, end - curr);
    return curr - result;
}

void song_free(Song *s)
{
    string_free(&s->filepath);
    string_free(&s->name);
}
