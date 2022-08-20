#pragma once
#include "layer.h"

Layer::Layer(int w, int h, cmrc::file::iterator* iter) {
	width = w;
	height = h;

	// weights
	weights.reserve(width);
	for (int ii = 0; ii < width; ii++) {
		vector<float> temp;
		temp.reserve(height);
		for (int jj = 0; jj < height; jj++) {
			float weight;
			memcpy(&weight, *iter, 4);
			*iter += 4;

			temp.push_back(weight);
		}
		weights.push_back(temp);
	}
	
	// biases
	bias.reserve(width);
	for (int ii = 0; ii < w; ii++) {
		float b;
		memcpy(&b, *iter, 4);
		*iter += 4;
		bias.push_back(b);
	}
	
}