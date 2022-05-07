# dupchess

The greatest chess engine ever.

Big thanks to GothamChess for the [shoutout](https://youtu.be/dITABWFb3qQ?t=10) (believe it or not, this is not Stockfish)

Move generation: Tested up to perft 7, approx 900,000 nodes/sec for release build on single core Ryzen 5 1600.  Higher depths will likely require multithreading to solve in a reasonable amount of time.
Also passes the perft tests from various positions in this [thread](http://www.talkchess.com/forum3/viewtopic.php?t=47318), which tests several edge cases.