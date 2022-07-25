#pragma once
#include "layer.h"
#include <fstream>
#include <vector>


using namespace std;
class NNUE
{
private:
	vector<Layer> layers;
public:
	NNUE(string weightFile);

};