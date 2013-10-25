#ifndef __locator_h__
#define __locator_h__

#include "dyn_array.h"
#include "song.h"

DYNAMIC_ARRAY_DECL(SongList, song_list, Song);

// absdirpath should not end with a /, which is the case when produced by
// canonicalize_file_name
int get_songs_abs(const char *absdirpath, SongList *sl);

#endif  // __locator_h__
