#pragma once
#include "layer.h"
#include "../chess/board.h"
#include <fstream>
#include <iostream>
#include <vector>


using namespace std;
class NNUE
{
private:
	vector<Layer> layers;
	vector<int> active_features;
	vector<float> accumulator;
	unsigned long wksq{};  // white king square
	unsigned long bksq{};  // black king square

	/// <summary>
	/// return the halfKP feature index for a given piece
	/// </summary>
	/// <param name="ksq">king square (0-63)</param>
	/// <param name="sq">piece square (0-63)</param>
	/// <param name="piece_type">type (01234, pnbrq)</param>
	/// <param name="color">piece color (0=white 1=black)</param>
	/// <returns></returns>
	uint32_t feature_index(int ksq, int sq, int piece_type, int color);

	// Clipped rectified linear unit activation
	void crelu(vector<float>& input);

public:
	NNUE();
	void get_active_features(const Board b);
	void refresh_accumulator();
	float eval();

};

