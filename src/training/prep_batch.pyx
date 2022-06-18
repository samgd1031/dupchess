
from libcpp.string cimport string

cdef extern from "prepare_batch_funcs.h":
	cdef int num_inputs();
	cdef string echo(string input, int* ii);
	cdef int feature_index(int ksq, int sq, int piece_type, int color);
	cdef int features_from_fen(const string fen, int* f_indexes, float* values, int king_color);
	
def num_inputs_p():
	return num_inputs()
	
	
def feature_index_p(ksq, sq, pt, color):
	return feature_index(ksq, sq, pt, color)
	

def features_from_fen_p(const string fen_str, int k_color):
	cdef int f_idxs[64]
	cdef float vals[64]
	return features_from_fen(fen_str, f_idxs, vals, k_color), f_idxs, vals


def echo_p(string input, int ii):
	return echo(input, &ii)
