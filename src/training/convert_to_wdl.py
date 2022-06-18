from pickletools import read_decimalnl_long
import sys
from tkinter.tix import MAX
import matplotlib.pyplot as plt
from pprint import pp, pprint
import math
import numpy as np
from scipy.optimize import curve_fit

CHUNK_SIZE = 1024*1024*10
MAX_CHUNKS = 10000
BUCKET_SIZE = 20  # bucket size in eval (centipawn I think?) space

def sigmoid(x, k):
    sig = 1.0 / (1.0 + np.exp(-x/k))
    return(sig)

def get_score_result(line):
    fen, eval, result = line.strip().split(', ')
    return int(eval), int(result)

def make_buckets(buckets, eval_res_pair):
    for pair in eval_res_pair:
        # find which bucket it belongs in
        # NOTE: Buckets are "named" by the lowest eval.  Example: bucket "0" holds evals from 0 to +BUCKET_SIZE
        bucket_ind = math.floor(pair[0] / BUCKET_SIZE) * BUCKET_SIZE

        # adjust result so that loss = 0, draw = 0.5, win =1
        # remember that results are always from white's perspective
        res = (pair[1] + 1) / 2
        
        # if bucket exists, add this result to the list
        if bucket_ind in buckets.keys():
            # weight current value by number of counts
            curr_avg = buckets[bucket_ind][0]
            count = buckets[bucket_ind][1]
            buckets[bucket_ind][0] = (count*curr_avg + res)/(count+1)
            buckets[bucket_ind][1] = count +1

        else:  # make a new key
            buckets[bucket_ind] = [res, 1]

    return buckets
    

def get_stats(datafile_path):
    chunk_num = 0
    buckets = dict()
    with open(datafile_path,'r') as df:
        while True:
            chunk = df.readlines(CHUNK_SIZE)
            if(len(chunk) == 0):
                break

            print(f"Chunk Number {chunk_num} - len {len(chunk)}")
            
            temp = [get_score_result(pos) for pos in chunk]
            buckets = make_buckets(buckets, temp)
            
            chunk_num += 1
            if chunk_num >= MAX_CHUNKS:
                break

        return buckets


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit(f"Usage {sys.argv[0]} <path_to_training_data>")
    
    bkts = get_stats(sys.argv[1])

    evals = np.fromiter(bkts.keys(), dtype=int)
    wdls = [bkts[key][0] for key in bkts.keys()]
    wdls = np.fromiter(wdls, dtype=float)
    
    sort_inds = np.argsort(evals)
    evals = np.take_along_axis(evals, sort_inds, 0)
    wdls = np.take_along_axis(wdls, sort_inds, 0)

    # fit curve
    popt, pcov = curve_fit(sigmoid, evals, wdls, p0=500)

    print(popt)

    plt.scatter(evals, wdls, label='data')
    plt.plot(evals, sigmoid(evals, *popt),'r--', label=f"sigmoid(x/{popt})")
    plt.xlabel('Evaulation Score')
    plt.ylabel("Win Probability")
    plt.legend(loc='best')
    plt.show()

