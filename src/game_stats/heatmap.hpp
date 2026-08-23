#ifndef CHESS_STATS_HEATMAP_HPP
#define CHESS_STATS_HEATMAP_HPP

#include "main.hpp"

StringUint32Map computeGameHeatmap(const GameData &game,
                                   chess::Color colorFilter);

emscripten::val computeHeatmap(const std::vector<GameData> &games,
                               chess::Color colorFilter);

#endif // CHESS_STATS_HEATMAP_HPP
