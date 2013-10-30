#ifndef __song_ratings_h__
#define __song_ratings_h__

#include <glib.h>
#include <time.h>

typedef enum
{
    SKIP,
    IGNORE,
    THUMBS_UP
} Action;

GString *action_to_str(Action *a);

typedef struct Rating
{
    Action a;
    // time wrt song start at which the action occurred
    // if IGNORE, then this is meaningless
    time_t song_time;
    // global time at which this action occurred
    time_t time;
} Rating;

// (Action, song_time, time)
GString *rating_to_str(Rating *r);

// SongRatings is an object to manage the current list of songs and their ratings
typedef struct SongRatings
{
    GHashTable *song_to_ratings;
} SongRatings;

void song_ratings_init(SongRatings *sr);
void song_ratings_add_song_name(SongRatings *sr, const gchar *name);
gboolean song_ratings_add_rating(SongRatings *sr, const gchar *name, const Rating *r);
GString *song_ratings_to_str(SongRatings *sr);
void song_ratings_free(SongRatings *sr);

#endif  // __song_ratings_h__
