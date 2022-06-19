from fileinput import filename
import torch
import prepare_batch as pb

class StreamLoader():
    def __init__(self, filepath, batch_size, num_batches, shuffle=False):
        self.batch_size = batch_size
        self.max_batches = num_batches
        self.max_samples = self.batch_size * self.max_batches

        self.current_samples = 0
        self.shuffle = shuffle

        self.file = filepath
        self.file_cursor = 0

        self.batches = []
        self.buf_cursor = 0

        self.is_eof = False

        self.refresh_buffer()  # load up buffer with batches

    def refresh_buffer(self):
        self.batches = []
        self.buf_cursor = 0
        for ii in range(self.max_batches):
            partial_batch, batch, f_cursor = pb.get_batch_p(self.batch_size, self.file, self.file_cursor)
            if partial_batch: # is EOF
                self.is_eof = True
                if ii < self.max_batches:
                    self.max_batches = ii
                break
            else:
                self.batches.append(batch)
                self.file_cursor = f_cursor

    def __iter__(self):
        return self

    def __next__(self):
        if self.buf_cursor < self.max_batches:
            self.buf_cursor += 1
            return self.batches[self.buf_cursor-1]
        elif not self.is_eof:  # havent reached EOF yet, refresh the buffer
            self.refresh_buffer()
            return self.__next__()
        else: # reached EOF, stop iterating
            raise StopIteration

