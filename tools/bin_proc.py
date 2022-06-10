# https://github.com/official-stockfish/Stockfish/blob/tools/src/extra/nnue_data_binpack_format.h

# process stockfish training data in the .bin format, and output pairs of fens and scores

import os
import bitstring


def process_move(move):
    info = {"flag": (move >> 14) & 0x02,
            "promo": (move >> 12) & 0x02,
            "from_sq": (move >> 6) & 63,
            "to_sq": move & 63}

    return info


def unpack_fen(packed):
    corrected = ""
    for ii in range(32):
        curr_byte = packed.bin[ii * 8:(ii + 1) * 8]
        le_byte = curr_byte[::-1]
        #print(f"{le_byte}  ", end='')
        corrected += le_byte
        #if (ii + 1) % 8 == 0:
            #print("")

    corrected = bitstring.ConstBitStream(bin=corrected)

    turn = "black" if corrected[0] else "white"
    #print(f"Turn: {turn}")

    white_king_index = corrected[6:0:-1].uint
    #print(f"white king: {white_king_index}")

    black_king_index = corrected[12:6:-1].uint
    #print(f"black king: {black_king_index}")
    #print("")

    counter = 0
    cursor = 13
    empty = 0
    fen = ""
    pieces = {"1000": "P",
              "1100": "N",
              "1010": "B",
              "1110": "R",
              "1001": "Q",
              }
    for rank in reversed(range(8)):
        for file in range(8):

            square_ind = rank * 8 + file
            if square_ind == white_king_index or square_ind == black_king_index:
                if empty > 0:
                    fen += str(empty)
                    empty = 0
                if square_ind == white_king_index:
                    #print('K', end='')
                    fen += 'K'
                else:
                    #print('k', end='')
                    fen += 'k'
            elif corrected[cursor] == 0:
                #print('.', end='')
                empty += 1
                cursor += 1
            else:
                if empty > 0:
                    fen += str(empty)
                    empty = 0

                piece = corrected[cursor:cursor + 4].bin
                piece = pieces[piece]
                color = corrected[cursor + 4]
                if color:
                    piece = piece.lower()
                #print(piece, end='')
                fen += piece
                cursor += 5

            counter += 1

        if empty > 0:
            fen += str(empty)
            empty = 0
        if rank > 0:
            fen += "/"
        else:
            fen += " "
        #print("")

    # turn to move
    if turn == "white":
        fen += "w"
    else:
        fen += "b"

    # castling
    castle_rights = corrected[cursor:cursor + 4].bin
    if castle_rights == "0000":
        fen += " -"
    else:
        for ii in range(4):
            if castle_rights[ii] == '1':
                fen += 'KQkq'[ii]

    cursor += 4

    # en passant
    ep = corrected[cursor + 6:cursor - 1:-1]
    is_ep = ep[-1]
    ep_ind = ep[:-2].uint

    if is_ep == 0:
        fen += " - "
        cursor += 1
    else:
        fen += " " + 'abcdefgh'[ep_ind % 8] + '12345678'[ep_ind // 8] + " "
        cursor += 7

    # 50 move counter
    f50 = corrected[cursor + 5:cursor - 1:-1].uint
    fen += f"{f50} "
    cursor += 6

    # full move counter
    fm = corrected[cursor + 15:cursor - 1:-1].uint
    fen += f"{fm}"

    assert (cursor <= 256)

    return fen


def to_text(move_dict):
    from_sq = move_dict["from_sq"]
    to_sq = move_dict["to_sq"]
    return 'abcdefgh'[from_sq % 8] + '12345678'[from_sq // 8] + 'abcdefgh'[to_sq % 8] + '12345678'[to_sq // 8]


def process_bitstring(bs):
    pfen = bs[0:32 * 8]  # 32 bytes for sfen
    score = bs[32 * 8:34 * 8].intle
    move = bs[34 * 8:36 * 8].uintle
    ply = bs[36 * 8:38 * 8].uintle
    result = bs[38 * 8:39 * 8].intle
    junk = bs[39 * 8:]

    return pfen, score, move, ply, result


if __name__ == '__main__':
    os.system('cls')
    with open("data__d9/combined/combined_dataset.bin", 'rb') as binfile:
        with open("data__d9/combined/combined_dataset_pr_w_result.dat", 'w') as outfile:
            num_bytes = 40
            content1 = binfile.read(num_bytes)
            pos = 1
            while len(content1) == 40:
                bs1 = bitstring.ConstBitStream(content1)
                pfen, score, move, ply, result = process_bitstring(bs1)

                move = process_move(move)
                movetext = to_text(move)

                fen = unpack_fen(pfen)
                if pos == 1 or pos % 10000 == 0:
                    print(f"pos {pos:12d}\t{fen:70s}\ts {score:5d}\tr: {result:2d}")

                outfile.write(f"{fen}, {score}, {result}\n")
                content1 = binfile.read(num_bytes)
                pos += 1
