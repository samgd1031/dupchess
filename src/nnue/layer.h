#pragma once
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;
class Layer 
{

public:
	Layer(int w, int h, ifstream* is);
	int width;
	int height;
	vector<vector<float>> weights;
	vector<float> bias;
};