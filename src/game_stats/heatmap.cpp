#include "common.hpp"
#include "heatmap.hpp"

using namespace chess;

StringUint32Map computeGameHeatmap(const GameData &game, Color colorFilter) {
  StringUint32Map heatmap;

  chess_stats::forEachOpeningPosition(
      game, colorFilter, chess_stats::ColorFilterMode::OPPOSITE,
      [&heatmap](const Board &, const ShortMove &sm) { ++heatmap[sm.to]; });

  return heatmap;
}
