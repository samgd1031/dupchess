'''

script to parse the lichess database (https://database.lichess.org/) and add positions from it
to the dataset used to train the neural network

The path to the folder where the lichess .bx2 files should be passed as a command line argument.

'''

import chess
import bz2
import pathlib
import sys
import os


def process_chunk(chunk):
    chunk_str = chunk.decode('utf-8').split('\n')
    game_start_ind = [ii for ii, x in enumerate(chunk_str) if x.startswith("[Event")]
    return game_start_ind
    

if __name__ == "__main__":
    # get list of all bz2 files in given folder
    lichess_db_folder = sys.argv[1]
    db_files = list(pathlib.Path(lichess_db_folder).glob('*.bz2'))

    # parse each bz2 file
    max_chunk_size = 1*1024*1024
    
    for file in db_files:
        bytes_processed = 0;
        file_size = os.path.getsize(file)
        with bz2.open(file, 'rb') as fid:
            for ii, chunk in enumerate(iter(lambda: fid.read(max_chunk_size), "")):
                print(process_chunk(chunk))
                bytes_processed += len(chunk)
                print(f"Chunk {ii+1:5d} - bytes processed: {bytes_processed/(1024*1024):.0f} MB")
            
                
