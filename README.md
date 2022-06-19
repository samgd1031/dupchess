# dupchess

The greatest chess engine ever.

Big thanks to GothamChess for the [shoutout](https://youtu.be/dITABWFb3qQ?t=10) (believe it or not, this is not Stockfish)

"Certified not egregious or weird"

## Move generation
Tested up to perft 7, approx 900,000 nodes/sec for release build on single core Ryzen 5 1600.  Higher depths will likely require multithreading to solve in a reasonable amount of time.
Also passes the perft tests from various positions in this [thread](http://www.talkchess.com/forum3/viewtopic.php?t=47318), which tests several edge cases.

## Evaluation
NNUE used for positional evaluation.  As implemented in this version, it will only look ahead at the next possible moves, making it a little shortsighted.  Alpha-beta search to come in the next version.

## Interface
Built to be UCI compliant, has played games successfully with Arena Chess GUI

## How to Play Against Dupchess
1. Download Arena Chess GUI [here](http://www.playwitharena.de/) and run the installer.  Any other UCI compliant GUI will also work but the rest of these instructions assume you're using Arena.
2. Download the version of Dupchess you want to play [here](https://github.com/samgd1031/dupchess/releases).  Save the .exe file somewhere on your computer and note the location for later.
3. Open Arena Chess GUI.
4. Select Engines>Install New Engine
5. Navigate to the dupchess.exe file you saved earlier.
6. For the type of engine, select UCI.  If asked to start the engine select "Yes".
7. Select File>New and start playing!