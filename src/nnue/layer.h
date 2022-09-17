#pragma once
#include <vector>

using namespace std;
class Layer 
{

public:
	Layer(int w, int h, vector<vector<float>> wgt, vector<float> bs);
	int width;	// output dimension
	int height; // input dimension
	vector<vector<float>> weights; // size: width x height 
	vector<float> bias; // size: width

	vector<float> forward(vector<float> input); // propagate through layer
};