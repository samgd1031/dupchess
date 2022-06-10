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

void process_packet(std::string pkt_data, std::bitset<32 * 8>* pfen) {
	*pfen = std::bitset<32 * 8>(pkt_data.substr(pkt_data.length() - 32 * 8));
};

std::string unpack_fen(std::string* packed) {
	std::string fen = "";

	//std::cout << *packed << std::endl;

	int side_to_move = std::stoi(packed->substr(255));
	std::cout << "side to move: " <<  side_to_move << std::endl;

	// king locations
	std::string white_king_bin = packed->substr(256 - 7, 6);
	std::string black_king_bin = packed->substr(256 - 13, 6);
	int white_king_ind = std::stoi(white_king_bin,nullptr, 2);
	int black_king_ind = std::stoi(black_king_bin,nullptr, 2);
	std::cout << "white king sq: " << white_king_ind << std::endl;
	std::cout << "black king sq: " << black_king_ind << std::endl;


	
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
	std::cout << "number of positions to process: " << num_positions << std::endl;


	uint16_t pos_size_bytes = 40;
	pos_packet pkt;

	input.seekg(0, std::ios::beg);
	input.read(reinterpret_cast<char*>(&pkt), sizeof(pkt));

	std::bitset<32 * 8> pack_fen;
	process_packet(pkt.data.to_string(), &pack_fen);

	std::string unpacked_fen = unpack_fen(&pack_fen.to_string());
	std::cout << unpacked_fen << std::endl;

	input.close();
	output.close();
}

