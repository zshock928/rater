#ifndef __song_h__
#define __song_h__

#include <glib.h>

typedef struct Song
{
    GString *filepath;
    GString *name;
} Song;

void song_init(Song *s, GString *filepath, GString *name);
GString *song_to_str(Song *s);
void song_free(Song *s);

#endif  // __song_h__
