#include "song.h"

void song_free(Song *s)
{
    string_free(&s->filepath);
    string_free(&s->name);
}
