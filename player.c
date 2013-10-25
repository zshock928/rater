#include "player.h"

#include "assert.h"

static GMainLoop *loop;

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
    gst_init(&argc, &argv);

    Player p;
    player_init(&p);

    // mimic an asynchronous request
    Song s;
    s.filepath = char_arr_to_string(
	"/home/fatty/Music/misc/05 I Can't Be Cool.mp3");
    s.name = char_arr_to_string("I Can't Be Cool");

    play(&p, &s);

    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    player_free(&p);
    g_main_loop_unref(loop);
    return 0;
}
