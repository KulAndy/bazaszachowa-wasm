#ifndef CHESS_STATS_PAWN_STRUCTURE_HPP
#define CHESS_STATS_PAWN_STRUCTURE_HPP

#include "main.hpp"

StringUint8Map computeGamePawnStruct(const GameData &game,
                                     chess::Color colorFilter);

#endif // CHESS_STATS_PAWN_STRUCTURE_HPP
