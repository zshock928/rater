#include <assert.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "song_ratings.h"

static const char * SKIP_STR = "boo";
static const char * IGNORE_STR = "meh";
static const char * THUMBS_UP_STR = "yay";

size_t action_to_str(Action *a, char *result, size_t maxlen)
{
    const char *str;
    switch (*a) {
    case SKIP:
        str = SKIP_STR;
        break;
    case IGNORE:
        str = IGNORE_STR;
        break;
    case THUMBS_UP:
        str = THUMBS_UP_STR;
        break;
    default:
        assert(false);
    }
    return add_str(str, result, maxlen);
}

size_t rating_to_str(Rating *ri, char *result, size_t maxlen)
{
    char *curr = result;
    char *end = result + maxlen;
    curr += add_str("(", curr, end - curr);
    curr += action_to_str(&ri->a, curr, end - curr);
    curr += add_str(", ", curr, end - curr);
    curr += strftime(curr, end - curr, "%Y-%m-%d %H:%M:%S", localtime(&ri->song_time));
    curr += add_str(", ", curr, end - curr);
    curr += strftime(curr, end - curr, "%Y-%m-%d %H:%M:%S", localtime(&ri->time));
    curr += add_str(")", curr, end - curr);
    return curr - result;
}

DYNAMIC_ARRAY_DEF(NameList, name_list, String, string_to_str, "[", ", ", "]",
		  string_free)
DYNAMIC_ARRAY_DEF(RatingList, rating_list, Rating, rating_to_str, "[", ", ",
		  "]", ((RatingListFreeFuncPtr)NULL))
DYNAMIC_ARRAY_DEF(RatingTable, rating_table, RatingList, rating_list_to_str,
		  "[", ", ", "]", rating_list_free)


void song_ratings_init(SongRatings *sr)
{
    sr->num_songs = 0;
    name_list_init(&sr->names);
    rating_table_init(&sr->ratings);
}

size_t song_ratings_add_song(SongRatings *sr, char *name)
{
    size_t new_id = sr->num_songs;
    String name_str = char_arr_to_string(name);
    size_t id = name_list_add(&sr->names, &name_str);
    assert(new_id == id);

    RatingList rl;
    rating_list_init(&rl);
    id = rating_table_add(&sr->ratings, &rl);
    assert(new_id == id);

    ++sr->num_songs;
    return new_id;
}

void song_ratings_add_rating(SongRatings *sr, size_t song_id, Rating *r)
{
    assert(song_id < sr->num_songs);
    assert(song_id < sr->ratings.count);
    rating_list_add(&sr->ratings.data[song_id], r);
}

size_t song_ratings_to_str(SongRatings *sr, char *result, size_t maxlen)
{
    assert(sr->num_songs == sr->names.count && sr->num_songs == sr->ratings.count);
    if (maxlen == 0) return 0;
    if (sr->num_songs == 0)
        return add_str("No songs in SongRatings", result, maxlen);

    size_t song_id = 0;
    String *curr_name = sr->names.data;
    RatingList *curr_rl = sr->ratings.data;
    char *curr = result;
    char *end = result + maxlen;
    while (song_id++ < sr->num_songs) {
        curr += add_strf("[%zu] \"", curr, end - curr, song_id);
        curr += string_to_str(curr_name++, curr, end - curr);
        curr += add_str("\"\n\t", curr, end - curr);
        curr += rating_list_to_str(curr_rl++, curr, end - curr);
        curr += add_str("\n", curr, end - curr);
    }
    return curr - result;
}

void song_ratings_free(SongRatings *sr)
{
    sr->num_songs = 0;
    name_list_free(&sr->names);
    rating_table_free(&sr->ratings);
}

int main(int argc, char *argv[])
{
    char result[1001] = {0};

    SongRatings sr;
    song_ratings_init(&sr);

    size_t id1 = song_ratings_add_song(&sr, "I Can't Be Cool");
    size_t id2 = song_ratings_add_song(&sr, "Canon in D");

    Rating r;
    r.a = THUMBS_UP;
    r.song_time = time(NULL);
    r.time = time(NULL);
    song_ratings_add_rating(&sr, id1, &r);

    r.a = IGNORE;
    r.song_time = time(NULL);
    r.time = time(NULL);
    song_ratings_add_rating(&sr, id2, &r);

    r.a = THUMBS_UP;
    r.song_time = time(NULL);
    r.time = time(NULL);
    song_ratings_add_rating(&sr, id1, &r);

    song_ratings_to_str(&sr, result, 1000);
    printf("%s\n", result);

    song_ratings_free(&sr);
    return 0;
}
