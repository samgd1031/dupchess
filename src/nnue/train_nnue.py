'''
defines the neural network in python so that training can be done with pytorch
'''

import torch
import torch.nn as nn
import os

INPUT_LAYER_SIZE = 40960*2  # halfKP encoding, kingSq * pieceSq * pieceType * color (64*64*5*2) times 2 --> 512
LAYER1_SIZE = 512  # 512 --> 32
LAYER2_SIZE = 32   # 32 --> 32
LAYER3_SIZE = 32   # 32 --> 1 (evaluation score)


class DupchessNNUE(nn.Module):
    def __init__(self):
        super(DupchessNNUE, self).__init__()
        self.input_layer = nn.Linear(INPUT_LAYER_SIZE, LAYER1_SIZE)
        self.L1 = nn.Linear(LAYER1_SIZE, LAYER2_SIZE)
        self.L2 = nn.Linear(LAYER2_SIZE, LAYER3_SIZE)
        self.L3 = nn.Linear(LAYER3_SIZE, 1)

    def forward(self, w_features, b_features):
        # Evaluation always from white perspective, different weights for white and black
        input = torch.cat([w_features, b_features], dim=1)
        accum = self.input_layer(input)  # white halfKP then black halfKP

        # put accumulator outputs into linear layers and use clamp for clipped ReLU
        L1_inp = torch.clamp(accum, 0.0, 1.0)
        L2_inp = torch.clamp(self.L1(L1_inp), 0.0, 1.0)
        L3_inp = torch.clamp(self.L2(L2_inp), 0.0, 1.0)

        # return last layer output (position evaluation), does not get clamped
        return self.L3(L3_inp)


if __name__ == "__main__":
    device = "cuda" if torch.cuda.is_available() else "cpu"
    print(f"Device Type: {device}")
