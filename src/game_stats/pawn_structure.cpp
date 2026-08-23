#include "common.hpp"
#include "pawn_structure.hpp"

#include <algorithm>

using namespace chess;

StringUint8Map computeGamePawnStruct(const GameData &game, Color colorFilter) {
  uint8_t isolatedPawns = 0;
  uint8_t doubledPawns = 0;
  uint8_t passedPawns = 0;
  uint8_t fianchettoPawns = 0;

  /*
   * The original implementation traversed the entire opening and
   * retained the LAST position matching colorFilter.
   *
   * We preserve that behavior here.
   */
  Board finalBoard;
  bool found = false;

  chess_stats::forEachOpeningPosition(
      game, colorFilter, chess_stats::ColorFilterMode::SAME,
      [&finalBoard, &found](const Board &board, const ShortMove &) {
        finalBoard = board;
        found = true;
      });

  if (!found) {
    return {{"isolated", 0}, {"doubled", 0}, {"passed", 0}, {"fianchetto", 0}};
  }

  auto pawnOnFile = [&finalBoard](Color side, uint8_t file) -> uint8_t {
    uint8_t count = 0;

    for (uint8_t rank = 0; rank < 8; ++rank) {
      const Square square = Square(File(file), Rank(rank));

      const Piece piece = finalBoard.at(square);

      if (piece != Piece::NONE && piece.type() == PAWN &&
          piece.color() == side) {
        ++count;
      }
    }

    return count;
  };

  for (uint8_t sideIndex = 0; sideIndex < 2; ++sideIndex) {
    const Color side = sideIndex == 0 ? Color::WHITE : Color::BLACK;

    for (uint8_t file = 0; file < 8; ++file) {
      const uint8_t count = pawnOnFile(side, file);

      // Doubled pawns
      if (count >= 2)
        doubledPawns += count - 1;

      // Isolated pawns
      if (count == 1) {
        const bool left = file > 0 && pawnOnFile(side, file - 1) > 0;

        const bool right = file < 7 && pawnOnFile(side, file + 1) > 0;

        if (!left && !right)
          ++isolatedPawns;
      }

      // Passed pawns
      if (count >= 1) {
        bool passed = true;

        const int minFile = std::max(0, static_cast<int>(file) - 1);

        const int maxFile = std::min(7, static_cast<int>(file) + 1);

        for (int enemyFile = minFile; enemyFile <= maxFile; ++enemyFile) {
          for (uint8_t rank = 0; rank < 8; ++rank) {
            const Square square = Square(File(enemyFile), Rank(rank));

            const Piece piece = finalBoard.at(square);

            if (piece != Piece::NONE && piece.type() == PAWN &&
                piece.color() != side) {
              passed = false;
              break;
            }
          }

          if (!passed)
            break;
        }

        if (passed)
          passedPawns += count;
      }
    }

    // Fianchetto pawns
    if (side == Color::WHITE) {
      if (finalBoard.at(Square::SQ_B3).type() == PAWN) {
        ++fianchettoPawns;
      }

      if (finalBoard.at(Square::SQ_G3).type() == PAWN) {
        ++fianchettoPawns;
      }
    } else {
      if (finalBoard.at(Square::SQ_B6).type() == PAWN) {
        ++fianchettoPawns;
      }

      if (finalBoard.at(Square::SQ_G6).type() == PAWN) {
        ++fianchettoPawns;
      }
    }
  }

  return {{"isolated", isolatedPawns},
          {"doubled", doubledPawns},
          {"passed", passedPawns},
          {"fianchetto", fianchettoPawns}};
}
