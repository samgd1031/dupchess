#include "nnue.h"

NNUE::NNUE(){
	init(WEIGHTFILE);
}

NNUE::NNUE(string weight_path) {
	init(weight_path);
}

void NNUE::init(string weight_path) {
	// open weights file
	ifstream in;
	in.open(weight_path, ios::binary);

	if (!in.is_open()) {
		std::cerr << "Error opening " << WEIGHTFILE
			<< " or file does not exist." << std::endl;
		return;
	}	


	// reserve active features (max 60, 30 non king pieces * 2 kings)
	active_features.reserve(60);

	// total number of layers in network
	uint32_t n_layers;
	in.read((char*)&n_layers, 4);
	//iter += 4;
	layers.reserve(n_layers);
	
	// get weights and biases for each layer
	for (uint32_t ii = 0; ii < n_layers; ii++) {
		uint32_t l_num, w, h;
		vector<vector<float>> weights;
		vector<float> biases;
		
		in.read((char*)&l_num, 4);  // layer number
		in.read((char*)&w, 4);  // width (output dimension)
		in.read((char*)&h, 4);  // height (input dimension)

		// read weights
		weights.reserve(w);
		for (uint32_t ii = 0; ii < w; ++ii)
		{
			vector<float> temp;
			temp.resize(h);
			in.read((char*) & temp[0], h * sizeof(float));
			weights.push_back(temp);
		}

		// read biases
		biases.resize(w);
		in.read((char*)&biases[0], w * sizeof(float));

		
		layers.push_back(Layer(w, h, weights, biases));
		
	}

	// reserve accumulator
	accumulator.reserve(layers[0].width);
	
}

/// <summary>
/// computes the accumulator from scratch (initialization or a king move)
/// </summary>
void NNUE::refresh_accumulator(){
	accumulator = layers[0].bias; // layer0 bias as starting point

	// add weight for each active feature
	for (int a : active_features)
	{
		for (int ii = 0; ii < layers[0].width; ++ii)
		{
			accumulator[ii] += layers[0].weights[ii][a];
		}
	}

}

/// <summary>
/// return the halfKP feature index for a given piece
/// </summary>
/// <param name="ksq">king square (0-63)</param>
/// <param name="sq">piece square (0-63)</param>
/// <param name="piece_type">type (01234, pnbrq)</param>
/// <param name="color">piece color (0=white 1=black)</param>
/// <returns></returns>
uint32_t NNUE::feature_index(int ksq, int sq, int piece_type, int color) {
	uint32_t p_idx = piece_type * 2 + color;
	return sq + (p_idx + ksq * 10) * 64;
}


/// <summary>
/// populate the list of active features given a game board
/// to be called on initialization
/// </summary>
/// <param name="b"></param>
void NNUE::get_active_features(const Board b) {
	active_features.clear(); // make sure active features is empty

	// Get king squares first
	bitscanfwd_u64(&wksq, b.state.kings & b.state.white_pcs);
	bitscanfwd_u64(&bksq, b.state.kings & b.state.black_pcs);

	// get number of pieces
	int n_pcs = popcount64(b.state.white_pcs | b.state.black_pcs);
	bitboard pcs = b.state.white_pcs | b.state.black_pcs;

	for (int ii = 0; ii < n_pcs; ++ii)
	{
		unsigned long psq; // piece square
		bitscanfwd_u64(&psq, pcs);

		// piece color
		int clr;
		if ((b.state.white_pcs >> psq) & 1ULL) { clr = (int)util::colors::WHITE; }
		else { clr = (int)util::colors::BLACK; }

		// piece type
		int ptype;
		if ((b.state.pawns >> psq) & 1ULL)			{ ptype = (int)util::Piece::PAWN; }
		else if ((b.state.knights >> psq) & 1ULL)	{ ptype = (int)util::Piece::KNIGHT; }
		else if ((b.state.bishops >> psq) & 1ULL)	{ ptype = (int)util::Piece::BISHOP; }
		else if ((b.state.rooks >> psq) & 1ULL)		{ ptype = (int)util::Piece::ROOK; }
		else if ((b.state.queens >> psq) & 1ULL)	{ ptype = (int)util::Piece::QUEEN; }
		else										{ ptype = (int)util::Piece::KING; }
		
		// do not need to make feature for king
		if (ptype != (int)util::Piece::KING) {
			active_features.push_back(feature_index(wksq, psq, ptype, clr));
			active_features.push_back(feature_index(bksq, psq, ptype, clr));
		}
		pcs &= ~(1ULL << psq);
	}
}



/// <summary>
/// propagate the accumulator through the remaining layers and return the evaluation
/// </summary>
/// <returns>evaluation as an integer number of centipawns</returns>
int NNUE::eval() {
	vector<float> input = accumulator;

	// propagate through hidden layers, applying clipped RELU activation
	vector<float> output;
	for (int ii = 1; ii < layers.size()-1; ++ii)
	{
		output = layers[ii].forward(input);
		crelu(output);
		input = output;
	}

	// last layer, do forward propagation but do not clip
	// this should result in a vector with one element in it
	vector<float> score = layers.back().forward(input);

	return (int)score[0]; // return just the value

}

void NNUE::crelu(vector<float>& input) {
	for (int ii = 0; ii < input.size(); ++ii)
	{
		input[ii] = min(max(input[ii], float(0)), float(1));
	}
}