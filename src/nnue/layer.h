#pragma once
#include <vector>
#include <cmrc/cmrc.hpp>

using namespace std;
class Layer 
{

public:
	Layer(int w, int h, cmrc::file::iterator* iter);
	int width;
	int height;
	vector<vector<float>> weights;
	vector<float> bias;
};