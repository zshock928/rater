#ifndef __locator_h__
#define __locator_h__

#include <glib.h>
#include "song.h"

// absdirpath should be the same path as when produced by canonicalize_file_name
// i.e. it should not end with a '/'
gint get_songs_abs(const gchar *absdirpath, GSList **songs);

#endif  // __locator_h__
