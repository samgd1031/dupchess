'''
Trains neural net using pytorch and c++/cython for loading training data
'''

import torch
from dupchess_nnue import DupchessNNUE
import prepare_batch as pb


if __name__ == "__main__":
    device = "cuda" if torch.cuda.is_available() else "cpu"
    print(f"Device Type: {device}")
    
    dc_nnue = DupchessNNUE()
    
    print(dc_nnue)
    print(pb.num_inputs_p())
    print(pb.feature_index_p(0, 8, 0, 0))
    
    