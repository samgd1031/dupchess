#pragma once
#include "layer.h"

Layer::Layer(int w, int h, vector<vector<float>> wgt, vector<float> bs) {
	width = w;
	height = h;

	// weights
	weights = wgt;
	
	// biases
	bias = bs;
	
}


/// <summary>
/// propagate input through layer (linear matrix multiplication)
/// </summary>
/// <param name="input"></param>
/// <returns></returns>
vector<float> Layer::forward(vector<float> input)
{
	// start with layer biases
	vector<float> output = bias;

	// add weights
	for (int ii = 0; ii < height; ++ii)
	{
		for (int jj = 0; jj < width; ++jj)
		{
			output[jj] += input[ii] * weights[jj][ii];
		}
	}

	return output;
}