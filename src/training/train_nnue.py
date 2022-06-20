'''
Trains neural net using pytorch and c++/cython for loading training data
'''

import torch
from dupchess_nnue import DupchessNNUE, INPUT_LAYER_SIZE
from streamloader import StreamLoader
import os
from pprint import pprint
import threading

'''
PARAMETERS FOR CONVERSION TO WIN/DRAW/LOSS SPACE
# run "convert_to_wdl.py" on training data to generate this value.
# converts the stockfish centipawn eval to win/draw/loss
# wdl = 1 / (1 + exp(-eval / FACTOR))
WDL_SIGMOID_FACTOR = 287

# when calcluating loss, this factor will be used to interpolate between the cp eval and wdl result of the position
# 0 = only use CP EVAL, 1.0 = only use WDL RESULT
WDL_INTERP_FACTOR = 0.5
'''

MAX_EPOCHS = 1
BATCH_SIZE = 10000
BUFFER_SIZE = 50 # batches to buffer in StreamLoader each with BATCH_SIZE samples

LEARNING_RATE = 0.5

PATH_TO_TRAINING_DATA = "D:/dupchess_data/stockfish_training_set/data__d9/combined/combined_dataset_processed.dat"

class loaderThread(threading.Thread):
    def __init__(self, name, loader, condition):
        super().__init__(daemon=True)
        self.name = name
        self.str_loader = loader
        
        self._cond = condition
        self.needs_update = True

    def run(self):
        while True:
            if self.needs_update:
                self.str_loader.refresh_buffer()
                self.needs_update = False
            else:
                with self._cond:
                    self._cond.notify()


        
if __name__ == "__main__":
    os.system('cls')
    
    lock = threading.Lock()
    condition = threading.Condition(lock=lock)

    device = "cuda" if torch.cuda.is_available() else "cpu"
    print(f"Device Type: {device}")
    
    print("intializing neural network...")
    dc_nnue = DupchessNNUE()
    criterion = torch.nn.MSELoss()
    optimizer = torch.optim.Adam(dc_nnue.parameters(), lr=LEARNING_RATE)
    print('Sending network to gpu')
    dc_nnue = dc_nnue.to(device)

    print('setting up stream to training data')
    train_dl = StreamLoader(PATH_TO_TRAINING_DATA, BATCH_SIZE, BUFFER_SIZE, shuffle=True)

    print("initializing training loader thread...")
    with condition:
        dload_thread = loaderThread('training loader', train_dl, condition)
        dload_thread.start()
    
        batch_iter = 1
        while not train_dl.is_eof:
            condition.wait()
            batch_group = train_dl.batches
            dload_thread.needs_update = True

            for features, sf_evals_cp in batch_group:
                if features is not None:
                    # send data to gpu
                    features = features.to(device)
                    sf_evals_cp = sf_evals_cp * 1.0
                    sf_evals_cp = sf_evals_cp.to(device)
                    #sf_evals_wdl = torch.sigmoid( sf_evals_cp / WDL_SIGMOID_FACTOR)
                    
                    # run features forward through network
                    model_eval_cp = dc_nnue.forward(features)
                    # convert from centipawn to win/draw/loss
                    #model_eval_wdl = torch.sigmoid(model_eval_cp / WDL_SIGMOID_FACTOR)

                    # loss function (mean squared error between dupchess and SF in WDL space)
                    # TODO: incorporate raw game result
                    # for now just uses centipawn score
                    loss = criterion(model_eval_cp, sf_evals_cp.reshape(BATCH_SIZE,1))
                    #loss = criterion(model_eval_wdl, sf_evals_wdl.reshape(BATCH_SIZE,1))

                    # backpropagate and optimize
                    optimizer.zero_grad()
                    loss.backward()
                    optimizer.step()

                    if batch_iter % 100 == 0:
                        print(f"step {batch_iter:8d} - loss: {loss.item():4f}")
                    batch_iter += 1
            