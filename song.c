#include "song.h"

#include <stdio.h>

void song_init(Song *s, GString *filepath, GString *name)
{
    s->filepath = filepath;
    s->name = name;
}

GString *song_to_str(Song *s)
{
    GString *result = g_string_new("");
    g_string_append(result, s->name->str);
    g_string_append(result, " (");
    g_string_append(result, s->filepath->str);
    g_string_append(result, ")");
    return result;
}

void song_free(Song *s)
{
    g_string_free(s->filepath, TRUE);
    g_string_free(s->name, TRUE);
}

// Testing
/*
gint main(gint argc, gchar *argv[])
{
    Song s;
    song_init(&s, g_string_new("song_name"), g_string_new("song_filepath"));
    GString *result = song_to_str(&s);
    puts(result->str);
    g_string_free(result, TRUE);
    song_free(&s);
    return 0;
}
*/
