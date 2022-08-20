#pragma once
#include "nnue.h"

CMRC_DECLARE(nnue_weights);

using namespace std;
NNUE::NNUE() {
	cmrc::embedded_filesystem fs = cmrc::nnue_weights::get_filesystem();
	cmrc::file wgt = fs.open("velma_v1.wgt");
	cmrc::file::iterator iter = wgt.begin();

	// total number of layers in network
	uint32_t n_layers;
	memcpy(&n_layers, iter, 4);
	iter += 4;
	layers.reserve(n_layers);
	
	// get weights and biases for each layer
	for (uint32_t ii = 0; ii < n_layers; ii++) {
		uint32_t l_num, w, h;
		memcpy(&l_num, iter, 4);  // layer number
		iter += 4;
		memcpy(&w, iter, 4);  // width (output dimension)
		iter += 4;
		memcpy(&h, iter, 4);  // height (input dimension)
		iter += 4;
	
		layers.push_back(Layer(w, h, &iter));
	}
	
}