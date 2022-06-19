
from libcpp.string cimport string
from libcpp.vector cimport vector
from libcpp cimport bool
from cython.parallel import prange
import torch

cdef extern from "prepare_batch_funcs.h":
	cdef int num_inputs();
	cdef string echo(string input, int* ii);
	cdef int feature_index(int ksq, int sq, int piece_type, int color);
	cdef long features_from_fen(const string fen, int* f_indexes, float* values);
	cdef bool get_batch(int num_samples, 
						const string filename, 
						unsigned long long* cursor, 
						vector[int] pos,
						vector[int] features,
						vector[float] values,
						vector[int] scores);
	
def num_inputs_p():
	return num_inputs()
	
	
def feature_index_p(ksq, sq, pt, color):
	return feature_index(ksq, sq, pt, color)
	

def features_from_fen_p(const string fen_str):
	cdef int f_idxs[60]
	cdef float vals[60]
	return features_from_fen(fen_str, f_idxs, vals), f_idxs, vals


def echo_p(string input, int ii):
	return echo(input, &ii)

def get_batch_p(num_samples, filename, start_index):
	cdef vector[int] pos, f_list, scores
	cdef vector[float] v_list
	cdef unsigned long long cursor_loc = start_index

	partial_batch = get_batch(num_samples, filename.encode('utf-8'), &cursor_loc, pos, f_list, v_list, scores)

	T = torch.sparse_coo_tensor(indices=[pos, f_list], values=v_list, size=(num_samples,40960*2))
	T.is_coalesced = True
	return partial_batch, (T, scores), cursor_loc
