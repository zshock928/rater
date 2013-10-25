#ifndef __song_h__
#define __song_h__

#include "dyn_array.h"

typedef struct Song
{
    String filepath;
    String name;
} Song;

size_t song_to_str(Song *s, char *result, size_t maxlen);
void song_free(Song *s);

#endif  // __song_h__
