#ifndef CHESS_STATS_ATTACKED_PIECES_HPP
#define CHESS_STATS_ATTACKED_PIECES_HPP

#include "main.hpp"

std::vector<uint8_t> computeGameAttackedPieces(const GameData &game,
                                               chess::Color colorFilter);

#endif // CHESS_STATS_ATTACKED_PIECES_HPP
