'''
Trains neural net using pytorch and c++/cython for loading training data
'''

import enum
from turtle import shape
import torch
from dupchess_nnue import DupchessNNUE, INPUT_LAYER_SIZE
from streamloader import StreamLoader
import numpy as np
import prepare_batch as pb
import cython
import os
from pprint import pprint


# run "convert_to_wdl.py" on training data to generate this value.
# converts the stockfish centipawn eval to win/draw/loss
# wdl = 1 / (1 + exp(-eval / FACTOR))
WDL_SIGMOID_FACTOR = 287

# when calcluating loss, this factor will be used to interpolate between the cp eval and wdl result of the position
# 0 = only use CP EVAL, 1.0 = only use WDL RESULT
WDL_INTERP_FACTOR = 0.5

MAX_EPOCHS = 1
BATCH_SIZE = 100000

PATH_TO_TRAINING_DATA = "D:/dupchess_data/stockfish_training_set/data__d9/combined/combined_dataset_processed.dat"



if __name__ == "__main__":
    os.system('cls')

    dev = "cuda" if torch.cuda.is_available() else "cpu"
    print(f"Device Type: {dev}")
    
    dc_nnue = DupchessNNUE()
    
    test_dl = StreamLoader(PATH_TO_TRAINING_DATA, 10000, 25, shuffle=False)

    for (ii, batch) in enumerate(test_dl):
        print(f"batch {ii+1} - {batch[0].size()}")

