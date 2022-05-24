'''

'''

import torch
import torch.nn as nn
import os

INPUT_LAYER_SIZE = 40960  # halfKP encoding, kingSq * pieceSq * pieceType * color (64*64*5*2)
LAYER1_SIZE = 256  # number of neurons in input layer (times 2 because it is split between white and black)
LAYER2_SIZE = 32
LAYER3_SIZE = 32


class DupchessNNUE(nn.Module):
    def __init__(self):
        super(DupchessNNUE, self).__init__()
        self.input_layer = nn.Linear(INPUT_LAYER_SIZE, LAYER1_SIZE)
        self.L1 = nn.Linear(LAYER1_SIZE * 2, LAYER2_SIZE)
        self.L2 = nn.Linear(LAYER2_SIZE, LAYER3_SIZE)
        self.L3 = nn.Linear(LAYER3_SIZE, 1)

    def forward(self, w_features, b_features, white_to_move):
        white = self.input_layer(w_features)  # white halfKP
        black = self.input_layer(b_features)  # black halfKP

        # white and black accumulators ordered based on who's turn it is
        # just a fancy way of doing this without if statement (white_to_move = 1 if whites turn, 0 otherwise)
        accum = (white_to_move * torch.cat([white, black], dim=1)) + \
                ((1 - white_to_move) * torch.cat([black, white], dim=1))

        # put accumulator outputs into linear layers and clamp for clipped ReLU
        L1_inp = torch.clamp(accum, 0.0, 1.0)
        L2_inp = torch.clamp(self.L1(L1_inp), 0.0, 1.0)
        L3_inp = torch.clamp(self.L2(L2_inp), 0.0, 1.0)

        # return last layer output (position evaluation), does not get clamped
        return self.L3(L3_inp)


if __name__ == "__main__":
    device = "cuda" if torch.cuda.is_available() else "cpu"
    print(f"Device Type: {device}")
