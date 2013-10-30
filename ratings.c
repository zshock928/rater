#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "ratings.h"

static const char * SKIP_STR = "boo";
static const char * IGNORE_STR = "meh";
static const char * THUMBS_UP_STR = "yay";

GString *action_to_str(Action *a)
{
    switch (*a) {
    case SKIP:
        return g_string_new(SKIP_STR);
    case IGNORE:
        return g_string_new(IGNORE_STR);
    case THUMBS_UP:
        return g_string_new(THUMBS_UP_STR);
    }
    assert(FALSE);
}

GString *rating_to_str(Rating *r)
{
    GString *result = g_string_new("(");

    GString *action = action_to_str(&r->a);
    g_string_append(result, action->str);
    g_string_free(action, TRUE);
    g_string_append(result, ", ");

    char datestr[256] = {0};
    strftime(datestr, 256, "%Y-%m-%d %H:%M:%S", localtime(&r->song_time));
    g_string_append(result, datestr);
    g_string_append(result, ", ");

    memset(datestr, '\0', 256);
    strftime(datestr, 256, "%Y-%m-%d %H:%M:%S", localtime(&r->time));
    g_string_append(result, datestr);
    g_string_append(result, ")");
    return result;
}

static guint g_gstr_hash(gconstpointer key)
{
    return g_str_hash(((GString *)key)->str);
}

static gboolean g_gstr_equal(gconstpointer a, gconstpointer b)
{
    return g_str_equal(((GString *)a)->str, ((GString *)b)->str);
}

void song_ratings_init(SongRatings *sr)
{
    sr->song_to_ratings = g_hash_table_new(
        g_gstr_hash,
        g_gstr_equal);
}

void song_ratings_add_song_name(SongRatings *sr, const gchar *name)
{
    guint old_size = g_hash_table_size(sr->song_to_ratings);

    // TODO check if song is already present
    GString *gs_name = g_string_new(name);
    GSList *ratings = NULL;
    g_hash_table_insert(sr->song_to_ratings, gs_name, ratings);

    assert(old_size + 1 == g_hash_table_size(sr->song_to_ratings));
}

gboolean song_ratings_add_rating(SongRatings *sr, const gchar *name, const Rating *r)
{
    GString *gs_name = g_string_new(name);
    GSList *ratings;
    gboolean present = g_hash_table_lookup_extended(
        sr->song_to_ratings, gs_name, NULL, (void **)&ratings);
    assert(present);

    // allocate a copy of rating to store in this object
    Rating *r_copy = g_new(Rating, 1);
    memcpy(r_copy, r, sizeof(Rating));
    GSList *new_ratings = g_slist_prepend(ratings, r_copy);

    g_hash_table_insert(sr->song_to_ratings, gs_name, new_ratings);
    g_string_free(gs_name, TRUE);
}

GString *song_ratings_to_str(SongRatings *sr)
{
    if (g_hash_table_size(sr->song_to_ratings) == 0)
        return g_string_new("No songs in SongRatings");

    GString *result = g_string_new("");
    GList *keys = g_hash_table_get_keys(sr->song_to_ratings);
    GList *key_it;
    for (key_it = keys; key_it; key_it = key_it->next) {
        g_string_append(result, "\"");
        g_string_append(result, ((GString *)key_it->data)->str);
        g_string_append(result, "\"\n\t");

        GSList *ratings;
        gboolean present = g_hash_table_lookup_extended(
            sr->song_to_ratings, key_it->data, NULL, (void **)&ratings);
        assert(present);
        GSList *rating_it = ratings;
        while (rating_it) {
            GString *gs_rating = rating_to_str(rating_it->data);
            g_string_append(result, gs_rating->str);
            g_string_free(gs_rating, TRUE);

            rating_it = rating_it->next;
            if (rating_it) g_string_append(result, ", ");
        }
        g_string_append(result, "\n");
    }
    g_list_free(keys);
    return result;
}

void song_ratings_free(SongRatings *sr)
{
    GList *keys = g_hash_table_get_keys(sr->song_to_ratings);
    GList *key_it;
    for (key_it = keys; key_it; key_it = key_it->next) {
        GSList *ratings;
        gboolean present = g_hash_table_lookup_extended(
            sr->song_to_ratings, key_it->data, NULL, (void**)&ratings);
        assert(present);
        GSList *rating_it;
        for (rating_it = ratings; rating_it; rating_it = rating_it->next) {
            g_free(rating_it->data);
        }
        g_slist_free(rating_it);

        g_hash_table_remove(sr->song_to_ratings, key_it->data);
        g_string_free(key_it->data, TRUE);
    }
    g_list_free(keys);
    g_hash_table_destroy(sr->song_to_ratings);
}

// Testing
/*
gint main(gint argc, gchar *argv[])
{
    SongRatings sr;
    song_ratings_init(&sr);

    const gchar *s1 = "I Can't Be Cool";
    const gchar *s2 = "Canon in D";

    song_ratings_add_song_name(&sr, s1);
    song_ratings_add_song_name(&sr, s2);

    Rating r;
    r.a = THUMBS_UP;
    r.song_time = time(NULL);
    r.time = time(NULL);
    song_ratings_add_rating(&sr, s1, &r);

    //sleep(1);

    r.a = IGNORE;
    r.song_time = time(NULL);
    r.time = time(NULL);
    song_ratings_add_rating(&sr, s2, &r);

    //sleep(1);

    r.a = THUMBS_UP;
    r.song_time = time(NULL);
    r.time = time(NULL);
    song_ratings_add_rating(&sr, s1, &r);

    //sleep(1);

    r.a = SKIP;
    r.song_time = time(NULL);
    r.time = time(NULL);
    song_ratings_add_rating(&sr, s2, &r);

    //sleep(1);

    r.a = THUMBS_UP;
    r.song_time = time(NULL);
    r.time = time(NULL);
    song_ratings_add_rating(&sr, s2, &r);

    GString *result = song_ratings_to_str(&sr);
    puts(result->str);
    g_string_free(result, TRUE);
    song_ratings_free(&sr);
    return 0;
}
*/
