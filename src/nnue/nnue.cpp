#pragma once
#include "nnue.h"

using namespace std;
NNUE::NNUE(string weightFile) {
	ifstream infile(weightFile, ios::binary);

	vector<char> buf;
	buf.resize(4);
	
	// total number of layers in network
	uint32_t n_layers;
	infile.read((char*)&n_layers, 4);
	layers.reserve(n_layers);

	// get weights and biases for each layer
	for (int ii = 0; ii < n_layers; ii++) {
		uint32_t l_num, w, h;
		infile.read((char*)&l_num, 4);  // layer number
		infile.read((char*)&w, 4);		// width (output dimension)
		infile.read((char*)&h, 4);		// height (input dimension)

		layers.push_back(Layer(w, h, &infile));
	}
}