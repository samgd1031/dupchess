'''

script to parse the lichess database (https://database.lichess.org/) and add positions from it
to the dataset used to train the neural network

'''

import chess
import bz2
import pathlib


MIN_AVG_ELO = 1500  # ignore games where the average elo is below this value
ELO_FLOOR = 1000    # ignore games where either player has an elo below this value

# get list of all bz2 files in given folder
lichess_db_folder = "D:/dupchess_data"
db_files = list(pathlib.Path(lichess_db_folder).glob('*.bz2'))

# parse each bz2 file
decomp = bz2.BZ2Decompressor()
for file in db_files:
    with open(file, mode='rb') as fid:
        data = b''
        while data == b'':
            data = decomp.decompress(fid.read(1))

        print(data.decode('utf-8'))
        for ii in range(1):
            data = decomp.decompress(fid.read(1))
            print(data.decode('utf-8'))