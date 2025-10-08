#include <iostream>
#include <vector>
#include <string>
#include "../../chess-library/include/chess.hpp"
#include <emscripten/bind.h>

using namespace chess;

std::string convert_uci_to_pgn(std::vector<std::string> uci_moves)
{
    Board board = Board::fromFen(constants::STARTPOS);
    std::vector<std::string> san_moves;

    for (const auto &uci : uci_moves)
    {
        const Move move = uci::uciToMove(board, uci);
        if (move == Move::NO_MOVE)
        {
            break;
        }
        san_moves.push_back(uci::moveToSan(board, move));
        board.makeMove(move);
    }

    std::string pgn = "";
    for (size_t i = 0; i < san_moves.size(); i++)
    {
        if (i % 2 == 0)
        {
            pgn += std::to_string(i / 2 + 1) + ".";
        }
        pgn += san_moves[i] + " ";
    }

    return pgn;
}

// 📌 Rejestracja std::vector<std::string> dla WebAssembly
EMSCRIPTEN_BINDINGS(chess_module)
{
    emscripten::register_vector<std::string>("VectorString");
    emscripten::function("convertUciToPgn", &convert_uci_to_pgn);
}
