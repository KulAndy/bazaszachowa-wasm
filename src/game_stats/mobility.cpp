#include "common.hpp"
#include "mobility.hpp"

using namespace chess;

std::vector<uint8_t> computeGameMobility(const GameData &game,
                                         Color colorFilter) {
  std::vector<uint8_t> mobility;

  chess_stats::forEachOpeningPosition(
      game, colorFilter, chess_stats::ColorFilterMode::OPPOSITE,
      [&mobility](const Board &board, const ShortMove &) {
        Movelist moves;
        movegen::legalmoves(moves, board);

        mobility.push_back(static_cast<uint8_t>(moves.size()));
      });

  return mobility;
}
