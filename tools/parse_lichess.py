'''

script to parse the lichess database (https://database.lichess.org/) and add positions from it
to the dataset used to train the neural network

The path to the folder where the lichess .bx2 files are should be passed as a command line argument.

'''

import chess.pgn
import bz2
import pathlib
import sys
import os
import io
import random
from multiprocessing import Pool

MIN_AVG_ELO = 2000  # ignore all games below this ELO
NUM_PROCESSES = 12  # number of parallel processes when initializing the threadpool
MAX_CHUNK_SIZE = 1024 * 1024  # max size of decompressed chunk, bytes


def process_chunk(data, pool):
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
    pos.extend(pool.map(process_game, pgn_strings))

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
            pos.append(current_node.board().fen())
            current_node = current_node.next()
        return pos


def flush_positions(pos, directory, source_filename):
    # create directory if it doesnt exist
    if not os.path.exists(directory):
        os.mkdir(directory)
    # name output file based on source
    outfile = source_filename[:len(source_filename)-8] + ".dat"
    with open(os.path.join(directory, outfile), 'a') as outf:
        for line in pos:
            outf.write(line + "\n")
    return []


if __name__ == "__main__":
    # get list of all bz2 files in given folder
    lichess_db_folder = sys.argv[1]
    db_files = list(pathlib.Path(lichess_db_folder).glob('*.bz2'))

    # parse each bz2 file
    num_flushed = 0
    with Pool(processes=NUM_PROCESSES) as pool:
        for file in db_files:
            file_size = os.path.getsize(file)
            with bz2.open(file, 'rb') as fid:
                positions = []
                for ii, chunk in enumerate(iter(lambda: fid.read(MAX_CHUNK_SIZE), "")):
                    positions.extend(set(process_chunk(chunk, pool)))
                    print(f"File: {os.path.split(file)[-1]} - Chunk {ii + 1:8d} - "
                          f"Total Positions: {num_flushed + len(positions):12d} - "
                          f"Buffer size {sys.getsizeof(positions)/1024:.0f} kB")
                    if sys.getsizeof(positions) / 1024 > 1024:
                        # shuffle positions before writing
                        random.shuffle(positions)
                        # flush to output
                        num_flushed += len(positions)
                        positions = flush_positions(positions,
                                                    os.path.join(lichess_db_folder, 'out'),
                                                    os.path.split(file)[-1])
