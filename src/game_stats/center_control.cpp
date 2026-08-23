#include "center_control.hpp"
#include "common.hpp"

using namespace chess;

std::vector<int8_t> computeGameCenterControl(const GameData &game,
                                             Color colorFilter) {
  std::vector<int8_t> centerControl;

  constexpr Square centerSquares[] = {Square::SQ_D4, Square::SQ_E4,
                                      Square::SQ_D5, Square::SQ_E5};

  chess_stats::forEachOpeningPosition(
      game, colorFilter, chess_stats::ColorFilterMode::SAME,
      [&centerControl, &centerSquares](const Board &board, const ShortMove &) {
        int8_t control = 0;

        for (const Square square : centerSquares) {
          const Piece piece = board.at(square);

          if (piece != Piece::NONE && piece.color() != board.sideToMove() &&
              piece.type() == PieceType::PAWN) {
            ++control;
          }
        }

        centerControl.push_back(control);
      });

  return centerControl;
}

std::vector<int8_t> computeGameExtendedCenterControl(const GameData &game,
                                                     Color colorFilter) {
  std::vector<int8_t> extendedCenterControl;

  constexpr Square extendedCenterSquares[] = {
      Square::SQ_C3, Square::SQ_D3, Square::SQ_E3, Square::SQ_F3,
      Square::SQ_C4, Square::SQ_D4, Square::SQ_E4, Square::SQ_F4,
      Square::SQ_C5, Square::SQ_D5, Square::SQ_E5, Square::SQ_F5,
      Square::SQ_C6, Square::SQ_D6, Square::SQ_E6, Square::SQ_F6};

  chess_stats::forEachOpeningPosition(
      game, colorFilter, chess_stats::ColorFilterMode::SAME,
      [&extendedCenterControl, &extendedCenterSquares](const Board &board,
                                                       const ShortMove &) {
        int8_t control = 0;

        for (const Square square : extendedCenterSquares) {
          const Piece piece = board.at(square);

          if (piece != Piece::NONE && piece.color() != board.sideToMove() &&
              piece.type() == PieceType::PAWN) {
            ++control;
          }
        }

        extendedCenterControl.push_back(control);
      });

  return extendedCenterControl;
}
