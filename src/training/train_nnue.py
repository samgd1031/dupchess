'''
Trains neural net using pytorch and c++/cython for loading training data
'''

from tkinter.tix import MAX
from typing import Type
import torch
from torch.utils.tensorboard import SummaryWriter
from dupchess_nnue import DupchessNNUE
import speedloader as sl
import os
import sys
import json
import threading


class loaderThread(threading.Thread):
    def __init__(self, name, filename, n_threads, n_batches, batch_size,condition):
        super().__init__(daemon=True)
        self.name = name
        self.filename = filename
        self.n_threads = n_threads
        self.n_batches = n_batches
        self.batch_size = batch_size
        self.input_features = []
        self.sf_evals = []
        self.results = []

        self.file_counter = 0
        self.is_eof = False
        
        self._cond = condition
        self.needs_update = True

    def run(self):
        while True:
            if self.needs_update:
                self.input_features, self.sf_evals, self.results, self.file_counter, self.is_eof = sl.loader_main_p(self.filename, 
                                                                                                       self.n_threads,
                                                                                                       self.n_batches,
                                                                                                       self.batch_size,
                                                                                                       self.file_counter)
                self.needs_update = False
            else:
                with self._cond:
                    self._cond.notify()

    def reset(self):
        self.file_counter = 0
        self.is_eof = False


        
if __name__ == "__main__":
    os.system('cls')
    
    if not len(sys.argv) == 2:
        print(f"Wrong number of command line arguments (received {len(sys.argv)-1}, should be 1)")
        print("\tArgument must be a file path to json file with model run options")
        sys.exit()

    lock = threading.Lock()
    condition = threading.Condition(lock=lock)

    device = "cuda" if torch.cuda.is_available() else "cpu"
    print(f"Device Type: {device}")

    
    print(f"Reading hyperparameters from {sys.argv[1]}")
    with open(sys.argv[1] ,'r') as jf:
        hp = json.load(jf)

    MAX_EPOCHS = hp["MAX_EPOCHS"]
    BATCH_SIZE = hp["BATCH_SIZE"]
    BUFFER_SIZE = hp["BUFFER_SIZE"]
    WDL_SIGMOID_FACTOR = hp["WDL_SIGMOID_FACTOR"]
    LEARNING_RATE = hp["LEARNING_RATE"]
    N_LOADER_THREADS = hp["N_LOADER_THREADS"]

    PATH_TO_TRAINING_DATA = hp["PATH_TO_TRAINING_DATA"]
    PATH_TO_VAL_DATA = hp["PATH_TO_VAL_DATA"]
    NUM_VAL_BATCHES = hp["NUM_VAL_BATCHES"]
    WEIGHTS_PATH = hp["WEIGHTS_PATH"]

    print(f"MAX_EPOCHS: {MAX_EPOCHS}\nBATCH_SIZE: {BATCH_SIZE} entries\nBUFFER_SIZE: {BUFFER_SIZE} batches\nN_LOADER_THREADS: {N_LOADER_THREADS}")

    print("intializing neural network...")
    dc_nnue = DupchessNNUE()
    dc_nnue = dc_nnue.to(device)
    if hp["LOSS_FUNCTION"] == "MSE":
        print("Loss function: Mean-Squared Error (MSE)")
        criterion = torch.nn.MSELoss()
    else:
        raise TypeError(f"'MSE' is only supported loss function at the moment, received '{hp['LOSS_FUNCTION']}'.")

    if hp["OPTIMIZER"] == "ADAM_W":
        print(f"Optimizer: AdamW with lr={LEARNING_RATE}")
        optimizer = torch.optim.AdamW(dc_nnue.parameters(), lr=LEARNING_RATE)
    elif hp["OPTIMIZER"] == "SGD":
        print(f"Optimizer: Stochastic Gradient Descent with lr={LEARNING_RATE}")
        optimizer = torch.optim.SGD(dc_nnue.parameters(), lr=LEARNING_RATE)
    else:
        raise TypeError(f"'ADAM_W' and 'SGD' are only supported optimizers at the moment, received '{hp['OPTIMIZER']}'.")

    print("Setting up Tensorboard writer...")
    writer = SummaryWriter("runs/")

    print("initializing training loader thread...")
    with condition:
        tr_loader = loaderThread('training loader', PATH_TO_TRAINING_DATA, N_LOADER_THREADS, BUFFER_SIZE, BATCH_SIZE, condition)
        tr_loader.start()
    
        print('Starting training run')
        total_iter = 1
        for epoch in range(1,MAX_EPOCHS+1):
            batch_iter = 1
            
            tr_loader.reset()
            while not tr_loader.is_eof:
                condition.wait()
                features = tr_loader.input_features
                evals = tr_loader.sf_evals
                res = tr_loader.results
                tr_loader.needs_update = True

                # normal training
                for ii in range(len(features)):
                    inpt = features[ii]
                    sf_evals_cp = evals[ii]
                    game_results = res[ii]

                    # send data to gpu
                    inpt = inpt.to(device)
                    sf_evals_cp = sf_evals_cp.to(device)
                    sf_evals_wdl = torch.sigmoid( sf_evals_cp / WDL_SIGMOID_FACTOR)


                    # run features forward through network
                    model_eval_cp = dc_nnue.forward(inpt)
                    # convert from centipawn to win/draw/loss
                    model_eval_wdl = torch.sigmoid(model_eval_cp / WDL_SIGMOID_FACTOR)

                    # loss function (mean squared error between dupchess and SF in WDL space)
                    # TODO: incorporate raw game result
                    loss = criterion(model_eval_wdl, sf_evals_wdl.reshape(BATCH_SIZE,1))

                    # backpropagate and optimize
                    optimizer.zero_grad()
                    loss.backward()
                    optimizer.step()

                    if batch_iter % BUFFER_SIZE == 0:
                        writer.add_scalar("Loss/train", loss.item(), total_iter)
                        print(f"epoch {epoch:<3d} - step {batch_iter:<8d} - loss: {loss.item():0.6f}")
                    batch_iter += 1
                    total_iter += 1

            
            # validate at end of epoch
            print(f"Validating at end of epoch {epoch}")
            with torch.no_grad():
                val_loader = loaderThread('validation', PATH_TO_VAL_DATA, N_LOADER_THREADS, BUFFER_SIZE, BATCH_SIZE, condition)
                val_loader.start()
                val_batches_processed = 0
                running_loss = 0
                while not val_loader.is_eof:
                    if val_batches_processed > NUM_VAL_BATCHES:
                        break

                    condition.wait()
                    features = val_loader.input_features
                    evals = val_loader.sf_evals
                    res = val_loader.results
                    val_loader.needs_update = True
                    
                    for ii in range(len(features)):
                        inpt = features[ii]
                        sf_evals_cp = evals[ii]
                        game_results = res[ii]

                        # send data to gpu
                        inpt = inpt.to(device)
                        sf_evals_cp = sf_evals_cp.to(device)
                        sf_evals_wdl = torch.sigmoid( sf_evals_cp / WDL_SIGMOID_FACTOR)

                        # run features forward through network
                        model_eval_cp = dc_nnue.forward(inpt)
                        # convert from centipawn to win/draw/loss
                        model_eval_wdl = torch.sigmoid(model_eval_cp / WDL_SIGMOID_FACTOR)

                        # loss function (mean squared error between dupchess and SF in WDL space)
                        # TODO: incorporate raw game result
                        loss = criterion(model_eval_wdl, sf_evals_wdl.reshape(BATCH_SIZE,1))
                        running_loss += loss.item()
                    
                    val_batches_processed += len(features)
                
                # average the loss over number of entries
                val_loss = running_loss / (val_batches_processed*BATCH_SIZE)
                print(f"Validation loss for epoch {epoch}: {val_loss:0.6f}")
                writer.add_scalar("Loss/validation", val_loss, total_iter)

            # save model parameters at end of epoch
            print(f"Saving model weights for epoch {epoch}")
            torch.save(dc_nnue.state_dict(), f'{WEIGHTS_PATH}/weights_ep{epoch:02d}.pth')
                
