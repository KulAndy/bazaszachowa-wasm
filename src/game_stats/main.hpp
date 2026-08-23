
#ifndef MAIN_H
#define MAIN_H

#include "../../chess-library/include/chess.hpp"
#include "../js/wrap.hpp"
#include "../types/GameData.hpp"
#include "../types/ShortMove.hpp"
#include "../types/TransparentStringHash.hpp"

using StringUint32Map =
    std::unordered_map<std::string, uint32_t, TransparentStringHash,
                       std::equal_to<>>;

using StringUint8Map =
    std::unordered_map<std::string, uint8_t, TransparentStringHash,
                       std::equal_to<>>;
                       
namespace std {

template <> struct hash<chess::Square> {
  size_t operator()(const chess::Square &square) const {
    return square.index();
  }
};

} // namespace std

#endif
