'''
Trains neural net using pytorch and c++/cython for loading training data
'''

import torch
from dupchess_nnue import DupchessNNUE
import speedloader as sl
import os
import threading


# PARAMETERS FOR CONVERSION TO WIN/DRAW/LOSS SPACE
# run "convert_to_wdl.py" on training data to generate this value.
# converts the stockfish centipawn eval to win/draw/loss
# wdl = 1 / (1 + exp(-eval / FACTOR))
WDL_SIGMOID_FACTOR = 287

'''
# when calcluating loss, this factor will be used to interpolate between the cp eval and wdl result of the position
# 0 = only use CP EVAL, 1.0 = only use WDL RESULT
WDL_INTERP_FACTOR = 0.5
'''

MAX_EPOCHS = 5
BATCH_SIZE = 100
BUFFER_SIZE = 5 # batches to buffer each with BATCH_SIZE samples

LEARNING_RATE = 0.01

#PATH_TO_TRAINING_DATA = "D:/dupchess_data/stockfish_training_set/data__d9/combined/combined_dataset_processed.dat"
#PATH_TO_TRAINING_DATA = "C:/dupchess_data/combined_dataset_processed.dat"
PATH_TO_TRAINING_DATA = "C:/Users/Sam/source/repos/dataloader/data/sample_1000_lines.dat"
N_LOADER_THREADS = 1

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


        
if __name__ == "__main__":
    os.system('cls')
    
    lock = threading.Lock()
    condition = threading.Condition(lock=lock)

    device = "cuda" if torch.cuda.is_available() else "cpu"
    print(f"Device Type: {device}")
    
    print("intializing neural network...")
    dc_nnue = DupchessNNUE()
    dc_nnue = dc_nnue.to(device)
    criterion = torch.nn.MSELoss()
    optimizer = torch.optim.SGD(dc_nnue.parameters(), lr=LEARNING_RATE)    

    print('setting up stream to training data')

    print("initializing training loader thread...")
    with condition:
        dload_thread = loaderThread('training loader', PATH_TO_TRAINING_DATA, N_LOADER_THREADS, BUFFER_SIZE, BATCH_SIZE, condition)
        dload_thread.start()
    
        print('Starting training run')
        for epoch in range(1,MAX_EPOCHS+1):
            batch_iter = 1
            
            while not dload_thread.is_eof:
                condition.wait()
                features = dload_thread.input_features
                evals = dload_thread.sf_evals
                res = dload_thread.results
                dload_thread.needs_update = True

                for ii in range(len(features)):
                    inpt = features[ii]
                    sf_evals_cp = evals[ii]
                    game_results = res[ii]

                    # send data to gpu
                    inpt = inpt.to(device)
                    sf_evals_cp = sf_evals_cp.to(device)
                    sf_evals_wdl = torch.sigmoid( sf_evals_cp / WDL_SIGMOID_FACTOR)


                    # run features forward through network
                    model_eval_cp = dc_nnue.forward(features)
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
                        print(f"epoch {epoch:<3d} - step {batch_iter:<8d} - loss: {loss.item():0.4f}")
                    batch_iter += 1

            # save model parameters at end of epoch
            torch.save(dc_nnue.state_dict(), f'/weights/weights_ep{epoch:02d}')
                
