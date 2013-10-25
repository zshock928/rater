#ifndef __player_h__
#define __player_h__

#include <stdbool.h>

#include <gst/gst.h>

#include "song.h"

typedef struct Player
{
    GstElement *play;
} Player;

gboolean player_init(Player *p);
void player_free(Player *p);

bool play(Player *p, Song *s);

#endif  // __player_h__
