#ifndef CHESS_STATS_MATERIAL_HPP
#define CHESS_STATS_MATERIAL_HPP

#include "main.hpp"

std::vector<int8_t> computeGameMaterialBalances(const GameData &game,
                                                chess::Color colorFilter);

#endif // CHESS_STATS_MATERIAL_HPP
