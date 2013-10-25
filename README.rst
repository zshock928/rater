Song Rater
==========

separate elements (threads?) that communicate through events

music player
------------
receives:

- song to play next
- whether to skip

does:

- what it says on the tin

sends:

- request for next song

notes:

command-line gui/controller
---------------------------
receives:

- what song is to be played

does:

- queries currently played song for position
- displays info on terminal

sends:

- rating action (skip/thumbs up)
- toggle repeat

rating logger
-------------
receives:

- rating action

does:

- initializes ratings (from logged source, see below); can be queried for these
- logs ratings somewhere

  * appending to csv is probably easiest - song name, rating; of course this
    assumes single app at a time (probably safe), and file names won't change
    (not safe)
  * database next, but requires mysql API for c; fixes first problem but not
    the second, so not sure if this is worthwhile
  * tagging in song - fixes second problem, but requires some protocol and I'm
    not sure how to do this in gstreamer

sends:

- nothing?

notes:

song locator
------------
receives:

does:

- finds location of all songs within a folder
- can be queried for song info

sends:

- nothing?

notes:

- use gstreamer to determine audio type/playable? probably easier to start with
  one type (mp3) and just look at file extension
- run in a separate thread checking file system for changes? inotify?
  g_io_add_watch?

  * if file changes occur, then some notification will need to sent out
  * need locking mechanisms

song selector
-------------
receives:

- rating action
- request for next song
- toggle repeat

does:

- queries for initial list of songs
- computes what next song should be upon request

sends:

- song to play next

notes:

- probably more efficient for this to contain and maintain current ratings,
  since it uses them directly and usually only one rating is updated between
  song requests

other notes/future goals:
-------------------------

- read lists of songs from online sources, randomize and stream

  * download song iff approved
  * will probably need some method to filter out duplicates (primitive url
    checking will probably handle most cases)
