# dupchess

The greatest chess engine ever.

Big thanks to GothamChess for the [shoutout](https://youtu.be/dITABWFb3qQ?t=10) (believe it or not, this is not Stockfish)

"Certified not egregious or weird"

## Move generation
Manually tested up to perft 7, approx 1,900,000 nodes/sec for release build on single core Ryzen 7 7700.  Higher depths will likely require multithreading to solve in a reasonable amount of time, TBD if that's worth implementing for move generation.
Also passes the perft tests from various positions in this [thread](http://www.talkchess.com/forum3/viewtopic.php?t=47318), which tests several edge cases.

Unit tests implement up to perft 5 from starting position, as well as all of the edge cases from the above thread.

## Evaluation
NNUE used for positional evaluation.  As implemented in this version, it will only look ahead at the next possible moves, making it a little shortsighted.  Alpha-beta (PV) search to come in the next version.
NNUE implemented in main engine code.  **TODO**: Implement incremental update to put the "UE" in "NNUE".  For now it doesnt really matter because the engine only looks one move ahead, so number of evaluations is small (tens of possible moves per turn)

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
