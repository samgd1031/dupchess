#pragma once

#include <fstream>
#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <assert.h>

using namespace std;

struct training_entry{
    vector<int> pos_idx;
    vector<int> feat_idx;
    vector<float> values;
};

void parallel_hello(int n_threads);
bool loader_main(string filename, int n_threads, int n_batches, int batch_size, vector<vector<int>>& pos,
                                                                                vector<vector<int>>& feats,
                                                                                vector<vector<float>>& vals,
                                                                                vector<int>& scores,
                                                                                vector<float>& results,
                                                                                uint64_t& counter);
bool load_from_file(std::ifstream* infile, int n_entries, vector<string>* fen_strs, vector<int>* scores, vector<float>* results);
inline void create_entry(training_entry* tr, int pos, string fen_str);
inline uint32_t feature_index(int ksq, int sq, int piece_type, int color);