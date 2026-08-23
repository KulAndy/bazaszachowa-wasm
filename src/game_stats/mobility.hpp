#ifndef CHESS_STATS_MOBILITY_HPP
#define CHESS_STATS_MOBILITY_HPP

#include "main.hpp"

std::vector<uint8_t> computeGameMobility(const GameData &game,
                                         chess::Color colorFilter);

#endif // CHESS_STATS_MOBILITY_HPP
