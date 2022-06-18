'''
Trains neural net using pytorch and c++/cython for loading training data
'''

import torch
from dupchess_nnue import DupchessNNUE
import prepare_batch as pb
import numpy as np
import cython
import os


# run "convert_to_wdl.py" on training data to generate this value.
# converts the stockfish centipawn eval to win/draw/loss
# wdl = 1 / (1 + exp(-eval / FACTOR))
WDL_SIGMOID_FACTOR = 287


if __name__ == "__main__":
    os.system('cls')

    device = "cuda" if torch.cuda.is_available() else "cpu"
    print(f"Device Type: {device}")
    
    dc_nnue = DupchessNNUE()
    
    #print(dc_nnue)
    

    fenstr = "6kq/pppppppp/8/8/8/8/pNBRQ3/K7 w - - 0 1".encode('utf-8')

    n_feats, feats, vals = pb.features_from_fen_p(fenstr, 0)
    print(feats[:n_feats])
    