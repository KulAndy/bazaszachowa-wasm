#ifndef CHESS_STATS_COMMON_HPP
#define CHESS_STATS_COMMON_HPP

#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>

#include "../../chess-library/include/chess.hpp"
#include "main.hpp"

namespace chess_stats {

constexpr uint8_t END_OPENING_HALF_MOVE = 50;

enum class ColorFilterMode { SAME, OPPOSITE };

inline std::string moveToUci(const ShortMove &sm) {
  std::string uci;
  uci.reserve(5);

  uci += sm.from;
  uci += sm.to;

  if (sm.promotion.has_value())
    uci += *sm.promotion;

  return uci;
}

template <typename Callback>
inline void forEachOpeningPosition(const GameData &game,
                                   chess::Color colorFilter,
                                   ColorFilterMode mode, Callback &&callback) {
  chess::Board board;
  board.setFen(chess::constants::STARTPOS);

  uint8_t counter = 0;

  for (const ShortMove &sm : game.moves) {
    const std::string uci = moveToUci(sm);
    const chess::Move move = chess::uci::uciToMove(board, uci);

    board.makeMove(move);

    const bool sameColor = board.sideToMove() == colorFilter;
    const bool selected =
        mode == ColorFilterMode::SAME ? sameColor : !sameColor;

    if (!selected) {
      continue;
    }

    callback(board, sm);

    if (++counter > END_OPENING_HALF_MOVE)
      break;
  }
}

} // namespace chess_stats

#endif // CHESS_STATS_COMMON_HPP
