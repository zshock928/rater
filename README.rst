Song Rater
==========

separate elements (threads?) that communicate through events

music player
------------
receives:

- song to play next

does:

- what it says on the tin

sends:

- rating action (skip/thumbs up/ignore)
- request for next song; this is always sent after rating action

notes:

- the act of rating/skipping could be a separate program? maybe communicate
  over a port? eventually stream from home? (security issues?)

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

song selector
-------------
receives:

- rating action
- request for next song

does:

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
