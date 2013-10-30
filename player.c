#include "player.h"

#include <assert.h>

#include "locator.h"

static GMainLoop *loop;
static GSList *songs, *curr_song_it;

static gboolean cb_error(GstBus *bus, GstMessage *message, gpointer data)
{
    Player *p = (Player *)data;
    GError *err;
    gchar *debug;

    gst_message_parse_error(message, &err, &debug);
    g_print("Error: %s\n", err->message);
    g_error_free(err);
    g_free(debug);

    g_main_loop_quit(loop);
    return TRUE;
}

static gboolean cb_eos(GstBus *bus, GstMessage *message, gpointer data)
{
    g_print("Finished playing song\n");

    Player *p = (Player *)data;
    gst_element_set_state((p->play), GST_STATE_READY);
    curr_song_it = curr_song_it->next;

    if (curr_song_it) {
        play(p, curr_song_it->data);
    } else {
        g_print("Finished playing all songs, exiting.\n");
        g_main_loop_quit(loop);
    }
    return TRUE;
}

gboolean player_init(Player *p)
{
    if (!(p->play = gst_element_factory_make("playbin", "play"))) {
	g_print("Failed to create element of type 'playbin'\n");
	return FALSE;
    }

    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(p->play));
    gst_bus_add_signal_watch(bus);
    g_signal_connect(bus, "message::error", G_CALLBACK(cb_error), p);
    g_signal_connect(bus, "message::eos", G_CALLBACK(cb_eos), p);
    gst_object_unref(bus);

}

void player_free(Player *p)
{
    gst_element_set_state(p->play, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(p->play));
}

gboolean play(Player *p, Song *s)
{
    printf("Playing %s\n", s->name->str);

    GString *uri = g_string_new("file://");
    g_string_append(uri, s->filepath->str);
    g_object_set(G_OBJECT(p->play), "uri", uri->str, NULL);
    gst_element_set_state((p->play), GST_STATE_PLAYING);
    g_string_free(uri, TRUE);
}

gint main(gint argc, gchar *argv[])
{
    gint err = get_songs_abs(argv[1], &songs);
    if (err) return err;
    if (songs == NULL) {
        fprintf(stderr, "No songs found in %s, exiting.\n", argv[1]);
        return 1;
    }
    curr_song_it = songs;
    gst_init(&argc, &argv);

    Player p;
    player_init(&p);

    // mimic an asynchronous request
    play(&p, curr_song_it->data);

    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    GSList *song_it;
    for (song_it = songs; song_it; song_it = song_it->next) {
        song_free(song_it->data);
        g_free(song_it->data);
    }
    g_slist_free(songs);
    player_free(&p);
    g_main_loop_unref(loop);
    return 0;
}

