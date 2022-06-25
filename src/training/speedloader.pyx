from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool

import torch

cdef extern from "load_batch_funcs.h":
    cdef void parallel_hello(int n_threads);
    cdef bool loader_main(string filename, int n_threads, int n_batches, int batch_size, 
                        vector[vector[int]] pos,
                        vector[vector[int]] feats,
                        vector[vector[float]] vals,
                        vector[int] scores,
                        vector[float] results,
                        int& counter);

def par_hello_p(n_threads):
    parallel_hello(n_threads)
    return None

def loader_main_p(file_n, n_threads, n_batches, batch_size, counter, shuffle=False, rgen=None):
    cdef vector[vector[int]] pos, f_list
    cdef vector[int] scores
    cdef vector [float] results
    cdef vector[vector[float]] v_list
    cdef int counter_c

    counter_c = counter

    is_eof = loader_main(file_n.encode('utf-8'), n_threads, n_batches, batch_size, pos, f_list, v_list, scores, results, counter_c)

    T = []
    for ii, p in enumerate(pos):
        num_samples = len(p)
        T.append(torch.sparse_coo_tensor(indices=[pos[ii], f_list[ii]], values=v_list[ii], size=(num_samples,40960*2)))

    return T, torch.tensor(scores), torch.tensor(results), counter_c, is_eof