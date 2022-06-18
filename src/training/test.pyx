

cdef extern from "prepare_batch.h":
	cdef int num_inputs();
	cdef int feature_index(int ksq, int sq, int piece_type, int color);
	cdef int features_from_fen(char* fen_c, int* f_indexes, float* values, int king_color);
	
def num_inputs_p():
	return num_inputs()
	
	
def feature_index_p(ksq, sq, pt, color):
	return feature_index(ksq, sq, pt, color)
	
'''	
def features_from_fen_p(fen_str, f_idxs, vals, k_color):
	return features_from_fen(fen_str, f_idxs, vals, k_color)
	'''