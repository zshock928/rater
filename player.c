#include "player.h"

#include "assert.h"
#include "locator.h"

static GMainLoop *loop;
static SongList sl;
static Song *curr_song;

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

    if (++curr_song >= sl.data + sl.count) {
        g_print("Finished playing all songs, exiting.\n");
        g_main_loop_quit(loop);
    } else {
        play(p, curr_song);
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

bool play(Player *p, Song *s)
{
    char uri[200] = "file://";
    char *curr = strchr(uri, '\0');
    char *end = uri + 200;
    assert(curr != NULL);
    string_to_str(&s->filepath, curr, end - curr);
    printf("Playing %s\n", uri);

    g_object_set(G_OBJECT(p->play), "uri", uri, NULL);
    gst_element_set_state((p->play), GST_STATE_PLAYING);
}

int main(int argc, char *argv[])
{
    song_list_init(&sl);
    get_songs_abs(argv[1], &sl);
    if (sl.count == 0) {
        song_list_free(&sl);
        fprintf(stderr, "No songs found in %s, exiting.\n", argv[1]);
        return 1;
    }
    curr_song = sl.data;

    gst_init(&argc, &argv);

    Player p;
    player_init(&p);

    // mimic an asynchronous request
    play(&p, curr_song);

    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    song_list_free(&sl);
    player_free(&p);
    g_main_loop_unref(loop);
    return 0;
}

