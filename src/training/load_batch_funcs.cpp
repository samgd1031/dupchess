#include "load_batch_funcs.h"


void parallel_hello(int n_threads)
{
	#pragma omp parallel num_threads (n_threads)
	{
		auto begin = std::chrono::high_resolution_clock::now();
		//printf("thread %d says hello\n", omp_get_thread_num());
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin);
		//printf("thread %d says I took %f ms to execute\n", omp_get_thread_num(), elapsed.count() / 1e6);
	}
}

bool loader_main(std::string filename, int n_threads, int n_batches, int batch_size, std::vector<std::vector<int>>& pos,
																					 std::vector<std::vector<int>>& feats,
																					 std::vector<std::vector<float>>& vals,
																					 std::vector<int>& scores,
																					 std::vector<float>& results,
																					 uint64_t& counter){
	// open file for reading
	std::ifstream infile;
	infile.open(filename, std::ios::in);
	if (!infile) { std::cout << "Error opening " << filename << "\n"; }
	infile.seekg(counter);
	
	// process file in parallel
	bool is_eof = false;
	#pragma omp parallel for num_threads (n_threads)
	for(int batch_num = 0; batch_num < n_batches; batch_num++)
	{
		// only one thread should read the file at a time
		bool should_process;
		std::vector<std::string> fens;
		std::vector<int> sf_evals;
		std::vector<float> batch_results;
		#pragma omp critical (file_read)
		{
			auto begin = std::chrono::high_resolution_clock::now();
			fens.reserve(batch_size);
			sf_evals.reserve(batch_size);
			batch_results.reserve(batch_size);

			is_eof = load_from_file(&infile, batch_size, &fens, &sf_evals, &batch_results);

			auto end = std::chrono::high_resolution_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
			//printf("Thread %d - I took %0.3f ms to read %zd lines (%0.3f us per line)\n", omp_get_thread_num(), elapsed.count() / 1e3, fens.size(), (float)elapsed.count() / batch_size);
			should_process = !is_eof; // assign here so that the value of is_eof isn't changed by anotehr thread
		}

		// processing can happen in parallel
		if (should_process) {
			auto begin = std::chrono::high_resolution_clock::now();
			//printf("Thread %d - Processing batch of size %d...\n", omp_get_thread_num(), batch_size);

			std::vector<int> pos_temp, feat_temp;
			std::vector<float> vals_temp;

			pos_temp.reserve(batch_size);
			feat_temp.reserve(batch_size);
			vals_temp.reserve(batch_size);

			for (int jj = 0; jj < batch_size; jj++) {
				training_entry entr;
				create_entry(&entr, jj, fens[jj]);

				pos_temp.insert(pos_temp.end(), entr.pos_idx.begin(), entr.pos_idx.end());
				feat_temp.insert(feat_temp.end(), entr.feat_idx.begin(), entr.feat_idx.end());
				vals_temp.insert(vals_temp.end(), entr.values.begin(), entr.values.end());
			}
			#pragma omp critical (push_batch) // only one thread should push to the main list at a time
			{
				pos.push_back(pos_temp);
				feats.push_back(feat_temp);
				vals.push_back(vals_temp);
				scores.insert(scores.end(), sf_evals.begin(), sf_evals.end());
				results.insert(results.end(), batch_results.begin(), batch_results.end());
			}
			auto end = std::chrono::high_resolution_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
			//printf("Thread %d - Done in %0.3f ms\n", omp_get_thread_num(), elapsed.count() / 1e3);

			
		}
	}
	counter = infile.tellg();
	return is_eof;
}


bool load_from_file(std::ifstream* infile, int n_entries, std::vector<std::string>* fen_strs, std::vector<int>* scores, std::vector<float>* results){
	std::string line;
	std::vector<std::string> splitstr;
	splitstr.reserve(3);
	if(infile->eof()){return true;}
	for(int ii = 0; ii < n_entries; ii++){
		if(infile->eof()){return true;}
		std::string tmp = "";
		std::getline(*infile, line);
		if (line.empty()) { return true; }
		splitstr.clear();
		for(int jj = 0; jj < line.length(); jj++)
		{
			if(line[jj] == ',') 			{ splitstr.push_back(tmp); tmp = ""; }
			else							{ tmp.push_back(line[jj]);}
		}
		splitstr.push_back(tmp);

		fen_strs->push_back(splitstr[0]);
		scores->push_back(std::stoi(splitstr[1]));
		results->push_back( (std::stof(splitstr[2]) + 1.0) / 2.0 );
		//std::cout << splitstr[0] << " | " << splitstr[1] << " | " << splitstr[2] << '\n';
	}
	return false;
}

inline void create_entry(training_entry* tr, int pos, std::string fen) {
	std::string tmp = "";
	std::vector<std::string> splitstr;

	// first, split up FEN string at spaces (based on training data format, should only be the position and color to move)
	size_t space_ind = fen.find(" ");
	splitstr.push_back(fen.substr(0, space_ind));
	fen.erase(0, space_ind+1);
	splitstr.push_back(fen);

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
	// white pieces first
	for (int ii = 0; ii < n_feat; ii++) {
		tr->feat_idx.push_back(feature_index(wk_sq, p_sq[ii], p_type[ii], p_color[ii]));
		tr->pos_idx.push_back(pos);
		tr->values.push_back(1.0f);
	}

	// then_black
	for (int ii = 0; ii < n_feat; ii++) {
		// add 40960 to "append" the black perspective to the white one
		tr->feat_idx.push_back(feature_index(bk_sq, p_sq[ii], p_type[ii], p_color[ii]) + 40960);
		tr->pos_idx.push_back(pos);
		tr->values.push_back(1.0f);
	}
}

inline uint32_t feature_index(int ksq, int sq, int piece_type, int color) {
	uint32_t p_idx = piece_type * 2 + color;
	return sq + (p_idx + ksq * 10) * 64;
}