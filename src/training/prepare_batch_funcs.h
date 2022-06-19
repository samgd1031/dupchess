#pragma once

#include <vector>
#include <assert.h>
#include <string>
#include <fstream>
#include <iostream>


int num_inputs();
std::string echo(const std::string input, int* ii);

/// <summary>
/// return the index for a given feature (piece on the board)
/// </summary>
/// <param name="ksq">King square (0-63)</param>
/// <param name="sq"> Piece square (0-63)</param>
/// <param name="piece_type"> 0=pawn, 1 = knight, 2 = bishop, 3 = rook, 4 = queen</param>
/// <param name="color">Color of the piece, 0 = white, 1 = black</param>
/// <returns> index of this feature in the halfKP encoding (0 - 40959)</returns>
int feature_index(int ksq, int sq, int piece_type, int color);

/// <summary>
/// given a fen string, create the list of feature indices for halfKP encoding
/// </summary>
/// <param name="fen"></param>
/// <param name="f_indexes"></param>
/// <param name="values"></param>
/// <param name="king_color"></param>
/// <returns></returns>
long features_from_fen(const std::string fen, int* f_indexes, float* values);

/// returns true if end of file, false otherwise
bool get_batch(int num_samples,
			   const std::string filename,
			   uint64_t* cursor, 
			   std::vector<int>& pos,
			   std::vector<int>& features,
			   std::vector<float>& values,
			   std::vector<int>& scores);