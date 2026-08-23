#include "common.hpp"
#include "material.hpp"

using namespace chess;

std::vector<int8_t> computeGameMaterialBalances(const GameData &game,
                                                Color colorFilter) {
  std::vector<int8_t> materialBalances;

  chess_stats::forEachOpeningPosition(
      game, colorFilter, chess_stats::ColorFilterMode::SAME,
      [&materialBalances](const Board &board, const ShortMove &) {
        int8_t white = 0;
        int8_t black = 0;

        for (Square sq = Square::SQ_A1; sq <= Square::SQ_H8; ++sq) {
          const Piece piece = board.at(sq);

          if (piece == Piece::NONE)
            continue;

          const int8_t value = -static_cast<int8_t>(piece.type());

          if (piece.color() == Color::WHITE)
            white += value;
          else
            black += value;
        }

        materialBalances.push_back(white - black);
      });

  return materialBalances;
}
