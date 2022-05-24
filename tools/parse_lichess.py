'''

script to parse the lichess database (https://database.lichess.org/) and add positions from it
to the dataset used to train the neural network

The path to the folder where the lichess .bx2 files are should be passed as the first command line argument.

'''

import chess
import chess.pgn
import bz2
import pathlib
import sys
import os
import io
import random
from multiprocessing import Pool

MIN_AVG_ELO = 2600  # ignore all games below this Elo
NUM_PROCESSES = 12  # number of parallel processes when initializing the threadpool
MAX_CHUNK_SIZE = 1024 * 1024  # max size of decompressed chunk, bytes
MAX_BUFFER_SIZE = 128 * 1024  # max number of positions to hold in memory before writing out to file, bytes

def process_chunk(data, parpool):
    data_str = data.decode('utf-8').split('\n')
    game_start_ind = [ind for ind, x in enumerate(data_str) if x.startswith("[Event")]
    pos = []
    pgn_strings = []
    for ii in range(len(game_start_ind) - 1):
        # create list of pgn strings from chunk data
        # as of right now, the program will ignore games that are split between chunks
        # (sorry for the unfortunate people whose games don't make it)
        pgn_strings.append("\n".join(data_str[game_start_ind[ii]:game_start_ind[ii + 1]]))

    # process all of the games
    pos.extend(parpool.map(process_game, pgn_strings))

    return flatten_list(pos)


def flatten_list(nested):
    flattened = []
    for el in nested:
        if type(el) is list:
            flattened.extend(el)
        else:
            flattened.append(el)
    return flattened


def process_game(pgn_str):
    pgn_io = io.StringIO(pgn_str)
    game = chess.pgn.read_game(pgn_io)
    current_node = game

    # check elo
    white_elo = int(game.headers.others["WhiteElo"])
    black_elo = int(game.headers.others["BlackElo"])
    if (white_elo + black_elo) / 2 < MIN_AVG_ELO:
        return []
    else:
        pos = []
        while current_node is not None:
            pos.append(current_node.board().fen().split(sep=" ")[0])
            current_node = current_node.next()
        return pos


def flush_positions(pos, directory, output_filename):
    # create directory if it doesnt exist
    if not os.path.exists(directory):
        os.mkdir(directory)
    with open(os.path.join(directory, output_filename), 'a') as outf:
        board = chess.Board()
        for line in pos:
            outf.write(f"{line}\n")
    return []


if __name__ == "__main__":
    # get list of all bz2 files in given folder
    lichess_db_folder = sys.argv[1]
    db_files = list(pathlib.Path(lichess_db_folder).glob('*.bz2'))

    # parse each bz2 file
    num_flushed = 0  # number of positions processed
    num_skipped = 0  # number of chunks skipped due to errors
    num_chunks = 0  # number of chunks processed overall
    with Pool(processes=NUM_PROCESSES) as pool:
        try:
            engine = chess.engine.SimpleEngine. \
                popen_uci("D:/Sam/Documents/chess/engines/stockfish_15_win_x64_avx2/stockfish_15_x64_avx2.exe")

            for file in db_files:
                output_filename = os.path.split(file)[-1]
                output_filename = output_filename[:len(output_filename) - 8] + ".dat"
                if os.path.isfile(os.path.join(lichess_db_folder, 'out', output_filename)):
                    print(f"{output_filename} exists, skipping.")
                    continue
                with bz2.open(file, 'rb') as fid:
                    positions = []
                    for ii, chunk in enumerate(iter(lambda: fid.read(MAX_CHUNK_SIZE), "")):
                        try:
                            positions.extend(set(process_chunk(chunk, pool)))
                            print(f"File: {os.path.split(file)[-1]} - Chunk {ii + 1:8d} - "
                                  f"Total Positions: {num_flushed + len(positions):12d} - "
                                  f"Buffer size {sys.getsizeof(positions) / 1024:.0f} kB")
                        except:
                            print(f"File: {os.path.split(file)[-1]} - Chunk {ii + 1:8d} - "
                                  f"Problem with chunk, skipping")
                            num_skipped += 1
                            raise
                        if sys.getsizeof(positions) > MAX_BUFFER_SIZE:
                            # shuffle positions before writing
                            random.shuffle(positions)
                            # flush to output
                            num_flushed += len(positions)
                            positions = flush_positions(positions,
                                                        os.path.join(lichess_db_folder, 'out'),
                                                        output_filename)
                        num_chunks += 1

        except KeyboardInterrupt:
            pool.close()
            print("keyboard interrupt. Exiting...")

    print(f"Processing complete.  {num_skipped} chunks out of {num_chunks} skipped due to errors.")
