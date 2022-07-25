#pragma once
#include "layer.h"

Layer::Layer(int w, int h, ifstream* is) {
	width = w;
	height = h;

	// weights
	weights.reserve(width);
	for (int ii = 0; ii < width; ii++) {
		vector<float> temp;
		temp.reserve(height);
		for (int jj = 0; jj < height; jj++) {
			float weight;
			is->read((char*)&weight, 4);

			temp.push_back(weight);
		}
		weights.push_back(temp);
	}
	
	// biases
	bias.reserve(width);
	for (int ii = 0; ii < w; ii++) {
		float b;
		is->read((char*)&b, 4);
		bias.push_back(b);
	}
	
}