#include "attacked_pieces.hpp"
#include "common.hpp"

#include <unordered_map>

using namespace chess;

std::vector<uint8_t> computeGameAttackedPieces(const GameData &game,
                                               Color colorFilter) {
  std::vector<uint8_t> attackedPieces;

  chess_stats::forEachOpeningPosition(
      game, colorFilter, chess_stats::ColorFilterMode::SAME,
      [&attackedPieces](const Board &board, const ShortMove &) {
        Movelist moves;

        movegen::legalmoves<movegen::MoveGenType::CAPTURE>(moves, board);

        std::unordered_map<Square, uint8_t> destCount;

        for (const Move &move : moves)
          ++destCount[move.to()];

        uint8_t count = 0;

        for (const auto &[square, attacks] : destCount) {
          (void)attacks;

          const Piece piece = board.at(square);

          if (piece != Piece::NONE && piece.color() != board.sideToMove()) {
            ++count;
          }
        }

        attackedPieces.push_back(count);
      });

  return attackedPieces;
}
