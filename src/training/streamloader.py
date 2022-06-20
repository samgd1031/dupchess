from fileinput import filename
import torch
import prepare_batch as pb

class StreamLoader():
    def __init__(self, filepath, batch_size, num_batches, shuffle=False):
        self.batch_size = batch_size
        self.max_batches = num_batches

        self.shuffle = shuffle
        if self.shuffle:
            self.rgen = torch.Generator()
            self.rgen.seed()
        else:
            self.rgen = None

        self.file = filepath
        self.file_cursor = 0

        self.batches = []

        self.is_eof = False

    def refresh_buffer(self):

        self.buf_cursor = 0
        n_new = self.max_batches - len(self)
        for ii in range(n_new):
            partial_batch, batch, f_cursor = pb.get_batch_p(self.batch_size, self.file, self.file_cursor, self.shuffle, self.rgen)
            if partial_batch: # is EOF
                self.is_eof = True
                break
            else:
                self.batches.append(batch)
                self.file_cursor = f_cursor

    def __len__(self):
        # return length of batch buffer
        return len(self.batches)

    def reset(self):
        self.file_cursor = 0
        self.is_eof = False