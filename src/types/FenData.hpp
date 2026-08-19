#include <emscripten/bind.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "./MoveData.hpp"
#include "./TransparentStringHash.hpp"

#ifndef FENDATA_H
#define FENDATA_H

struct FenData {
  std::unordered_map<std::string, MoveData, TransparentStringHash,
                     std::equal_to<>>
      moves;

  std::vector<int> indexes;

  emscripten::val getMovesJS() const {
    emscripten::val result = emscripten::val::object();

    for (const auto &[key, moveData] : moves) {
      emscripten::val moveObj = emscripten::val::object();
      moveObj.set("games", moveData.games);
      moveObj.set("points", moveData.points);

      emscripten::val yearsArray = emscripten::val::array();
      for (size_t i = 0; i < moveData.years.size(); ++i) {
        yearsArray.set(i, moveData.years[i]);
      }
      moveObj.set("years", yearsArray);

      emscripten::val statsMap = emscripten::val::object();

      for (const auto &[statKey, statData] : moveData.stats) {
        emscripten::val statObj = emscripten::val::object();
        statObj.set("count", statData.count);
        statObj.set("points", statData.points);
        statsMap.set(std::to_string(statKey), statObj);
      }

      moveObj.set("stats", statsMap);
      result.set(key, moveObj);
    }

    return result;
  }

  emscripten::val getIndexesJS() const {
    emscripten::val arr = emscripten::val::array();
    size_t i = 0;

    for (const auto &index : indexes) {
      arr.set(i++, index);
    }

    return arr;
  }
};

#endif
