import pandas as pd

# shuffles training data (since it comes somewhat ordered from stockfish)

infile = r"C:\Users\Sam\source\dupchess_data\stockfish_training_set\velma__depth9\combined\combined_out.dat"

if __name__ == '__main__':
    df = pd.read_table(infile)

    print(df)
    df = df.sample(frac=1).reset_index(drop=True)
    print(df)

    outfile = infile[:-4] + "_shuffled.dat"
    df.to_csv(outfile, sep='\n', index=False)