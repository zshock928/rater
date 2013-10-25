#ifndef __song_ratings_h__
#define __song_ratings_h__

#include "dyn_array.h"

typedef enum
{
    SKIP,
    IGNORE,
    THUMBS_UP
} Action;

size_t action_to_str(Action *a, char *result, size_t maxlen);

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
size_t rating_to_str(Rating *ri, char *result, size_t maxlen);

DYNAMIC_ARRAY_DECL(NameList, name_list, String);
DYNAMIC_ARRAY_DECL(RatingList, rating_list, Rating);
DYNAMIC_ARRAY_DECL(RatingTable, rating_table, RatingList);

typedef struct SongRatings
{
    size_t num_songs;
    NameList names;
    RatingTable ratings;
} SongRatings;

void song_ratings_init(SongRatings *sr);
size_t song_ratings_add_song(SongRatings *sr, char *name);
void song_ratings_add_rating(SongRatings *sr, size_t song_id, Rating *r);
size_t song_ratings_to_str(SongRatings *sr, char *result, size_t maxlen);
void song_ratings_free(SongRatings *sr);

#endif  // __song_ratings_h__
