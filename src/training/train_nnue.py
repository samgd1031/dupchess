'''
Trains neural net using pytorch and c++/cython for loading training data
'''

import torch
from dupchess_nnue import DupchessNNUE
import prepare_batch as pb
import numpy as np
import cython
import os


if __name__ == "__main__":
    os.system('cls')

    device = "cuda" if torch.cuda.is_available() else "cpu"
    print(f"Device Type: {device}")
    
    dc_nnue = DupchessNNUE()
    
    #print(dc_nnue)
    

    fenstr = "6kq/pppppppp/8/8/8/8/pNBRQ3/K7 w - - 0 1".encode('utf-8')

    n_feats, feats, vals = pb.features_from_fen_p(fenstr, 0)
    print(feats[:n_feats])
    