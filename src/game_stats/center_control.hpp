#ifndef CHESS_STATS_CENTER_CONTROL_HPP
#define CHESS_STATS_CENTER_CONTROL_HPP

#include "main.hpp"

std::vector<int8_t> computeGameCenterControl(const GameData &game,
                                             chess::Color colorFilter);

std::vector<int8_t> computeGameExtendedCenterControl(const GameData &game,
                                                     chess::Color colorFilter);

#endif // CHESS_STATS_CENTER_CONTROL_HPP
