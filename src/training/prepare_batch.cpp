#include "prepare_batch.h"


int num_inputs() {
	return 64 * 64 * 5 * 2;
}

std::string echo(const std::string input, int* ii){
	return input + " " + std::to_string(*ii);
}

/// <summary>
/// return the index for a given feature (piece on the board)
/// </summary>
/// <param name="ksq">King square (0-63)</param>
/// <param name="sq"> Piece square (0-63)</param>
/// <param name="piece_type"> 0=pawn, 1 = knight, 2 = bishop, 3 = rook, 4 = queen</param>
/// <param name="color">Color of the piece, 0 = white, 1 = black</param>
/// <returns> index of this feature in the halfKP encoding (0 - 40959)</returns>
int feature_index(int ksq, int sq, int piece_type, int color) {
	int p_idx = piece_type * 2 + color;
	return sq + (p_idx + ksq * 10) * 64;
}

/// <summary>
/// given a fen string, create the list of feature indices for halfKP encoding
/// </summary>
/// <param name="fen"></param>
/// <param name="f_indexes"></param>
/// <param name="values"></param>
/// <param name="king_color"></param>
/// <returns></returns>
int features_from_fen(const std::string fen, int* f_indexes, float* values, int king_color){
	std::string tmp = "";
	std::vector<std::string> splitstr;

	// first, split up FEN string at spaces (based on training data format, should only be the position and color to move)
	for (int ii = 0; ii < fen.length(); ii++) {
		if (fen[ii] == ' ') {
			splitstr.push_back(tmp);
			tmp = "";
		}
		else {
			tmp.push_back(fen[ii]);
		}
	}
	splitstr.push_back(tmp);  // add last part of FEN string to splitstr

	// process position
	std::vector<std::string> fen_ranks;
	tmp = "";
	for (int ii = 0; ii < splitstr[0].length(); ii++) {
		if (splitstr[0][ii] == '/') { fen_ranks.push_back(tmp); tmp = ""; }
		else { tmp.push_back(splitstr[0][ii]); }
	}
	fen_ranks.push_back(tmp);
	// make sure there are 8 ranks
	assert(fen_ranks.size() == 8);
		
	// process ranks 1-8 so that features are ordered
	int n_feat = 0;
	int wk_sq, bk_sq; // king square indexes
	int p_color[30], p_type[30], p_sq[30];

	for (int rr = 0; rr < 8; rr++) {
		std::string frank = fen_ranks[7-rr];
		int sqind = rr * 8;
		for (int ii = 0; ii < frank.length(); ii++) {
			char c = frank[ii];

			if (tolower(c) == 'k') { // handle kings differently, store square for later
				if (isupper(c)) { wk_sq = sqind; }
				else { bk_sq = sqind; }
				sqind++;
			}
			else if (isalpha(c)) { // non king piece, need to create a feature for it
				// color
				if (isupper(c)) { p_color[n_feat] = 0; }
				else { p_color[n_feat] = 1; }
					
				// piece type
				switch (tolower(c))
				{
				case 'p':
					p_type[n_feat] = 0;
					break;
				case 'n':
					p_type[n_feat] = 1;
					break;
				case 'b':
					p_type[n_feat] = 2;
					break;
				case 'r':
					p_type[n_feat] = 3;
					break;
				case 'q':
					p_type[n_feat] = 4;
					break;
				default: // you messed up
					p_type[n_feat] = 9999999; // should cause problems with indexing later, this is probably a horrible way to handle errors
					break;
				}
				p_sq[n_feat] = sqind;

				sqind++;
				n_feat++;
			}
			else if (isdigit(c)) { sqind += c - 48; } // empty spaces, convert digit character to number by subtracting 48 (ASCII '0')
		}
	}

	// build features now that we know all king and piece squares
	int king_sq;
	if (king_color == 0) { king_sq = wk_sq; }
	else { king_sq = bk_sq; }

	for (int ii = 0; ii < n_feat; ii++) {
		f_indexes[ii] = feature_index(king_sq, p_sq[ii], p_type[ii], p_color[ii]);
		values[ii] = 1.0f;
	}

	return n_feat;
}