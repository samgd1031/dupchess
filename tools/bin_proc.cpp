#include <stdexcept>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <bitset>
#include <string>


struct pos_packet
{
	std::bitset<40 * 8> data;
};

enum class Piece { PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING, NONE };
static const char* pieceAbbr[6]{ "p","b","n","r","q","k" };


void process_packet(std::string pkt_data, std::bitset<32 * 8>* pfen, 
					short* sc,
					short* res) 
{
	// packed fen (to be unpacked in another function)
	*pfen = std::bitset<32 * 8>(pkt_data.substr(pkt_data.length() - 32 * 8));
	
	// stockfish evaluation
	std::bitset<2 * 8> temp = std::bitset<2 * 8>(pkt_data.substr(pkt_data.length() - 34 * 8, 16));
	*sc = static_cast<short>(temp.to_ulong());

	// game result (win lose draw)
	std::bitset<8> temp2 = std::bitset<8>(pkt_data.substr(pkt_data.length() - 39 * 8, 8));
	*res = static_cast<short>(temp2.to_ulong());
	if (*res == 255) { *res = -1; }
};

std::string unpack_fen(std::string* packed) {
	std::string fen = "";

	int side_to_move = std::stoi(packed->substr(255));

	// king locations
	std::string white_king_bin = packed->substr(256 - 7, 6);
	std::string black_king_bin = packed->substr(256 - 13, 6);
	int white_king_ind = std::stoi(white_king_bin,nullptr, 2);
	int black_king_ind = std::stoi(black_king_bin,nullptr, 2);

	// other piece positions
	int cursor = 255 - 13;
	int empty_count = 0;
	for (int rank = 7; rank >= 0; rank--) {
		for (int file = 0; file < 8; file++) {
			int sq_ind = rank * 8 + file;

			std::string temp = packed->substr(cursor, 1);
			// king index
			if ((sq_ind == white_king_ind) || (sq_ind == black_king_ind)) {
				if (empty_count > 0) { fen += std::to_string(empty_count); empty_count = 0; }
				if (sq_ind == white_king_ind) {
					fen += "K";
				}
				else {
					fen += "k";
				}
			}
			// empty space
			else if (temp.compare("0") == 0) {
				empty_count++;
				cursor -= 1;
			}
			// non king piece
			else {
				if (empty_count > 0) { fen += std::to_string(empty_count); empty_count = 0; }
				std::string piece = packed->substr(cursor - 3, 4);
				std::string color = packed->substr(cursor - 4, 1);
				std::string fpiece = "";
				if (piece.compare("0001") == 0) { fpiece = "p"; }
				else if (piece.compare("0011") == 0) { fpiece = "n"; }
				else if (piece.compare("0101") == 0) { fpiece = "b"; }
				else if (piece.compare("0111") == 0) { fpiece = "r"; }
				else if (piece.compare("1001") == 0) { fpiece = "q"; }
				else { fpiece = "x"; std::cout << piece; } // should never be reached
				if (color.compare("0") == 0) { fpiece = std::toupper(fpiece[0]); }
				
				fen += fpiece;
				cursor -= 5;
			}
		}
		if (empty_count > 0) { fen += std::to_string(empty_count); empty_count = 0; }
		if (rank > 0) { fen += "/"; }	
	}

	// side to move
	if (side_to_move == 0) { fen += " w"; }
	else { fen += " b"; }
	
	// dont really need the rest, only need board position (and maybe side to move)

	return fen;
}

int main(int argc, char** argv)
{
	if (argc != 3) {
		std::cout << "requires two arguments <inputfile> <outputfile>" << std::endl;
		std::cout << "received: " << argc << std::endl;
		return -1;
	}

	for (int ii = 0; ii < argc; ii++) {
		std::cout << argv[ii] << std::endl;
	}

	std::ifstream input(argv[1], std::ios::binary);
	std::ofstream output(argv[2], std::ios::trunc);

	uintmax_t fs = std::filesystem::file_size(argv[1]);
	uintmax_t num_positions = fs / 40;

	std::cout << "file_size: " << fs << std::endl;
	std::cout << "number of positions to process: " << num_positions << "\n\n";


	uint16_t pos_size_bytes = 40;
	pos_packet pkt;

	input.seekg(0, std::ios::beg);
	for (int ii = 0; ii < num_positions; ii++) {
		input.read(reinterpret_cast<char*>(&pkt), sizeof(pkt));

		std::bitset<32 * 8> pack_fen;
		short score, result;
		process_packet(pkt.data.to_string(), &pack_fen, &score, &result);

		std::string unpacked_fen = unpack_fen(&pack_fen.to_string());

		if ((ii == 0) || (ii+1) % 10000 == 0) {
			std::cout << "pos " << std::setw(12) << ii + 1 << "\t";
			std::cout << std::setw(72) << unpacked_fen << "\tsc ";
			std::cout << std::setw(5) << score << "\tr ";
			std::cout << std::setw(2) << result << std::endl;
		}

		output << unpacked_fen << ", " << score << ", " << result << std::endl;

	}
	

	input.close();
	output.close();
}

