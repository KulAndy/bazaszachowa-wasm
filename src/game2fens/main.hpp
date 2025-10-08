
#ifndef MAIN_H
#define MAIN_H

#include "main.hpp"
#include <emscripten/bind.h>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

struct Stats {
  int count;
  float points;
};

class ShortMove {
public:
  std::string from;
  std::string to;
  std::optional<std::string> promotion;

  ShortMove(const std::string &from, const std::string &to,
            const std::optional<std::string> &promotion = std::nullopt)
      : from(from), to(to), promotion(promotion) {}
};

struct MoveData {
  int games;
  float points;
  std::vector<int> years;
  std::unordered_map<int, Stats> stats;
};

struct FenData {
  std::unordered_map<std::string, MoveData> moves;
  std::vector<int> indexes;

  emscripten::val getMovesJS() const {
    emscripten::val result = emscripten::val::object();
    for (const auto &pair : moves) {
      emscripten::val moveObj = emscripten::val::object();
      moveObj.set("games", pair.second.games);
      moveObj.set("points", pair.second.points);

      emscripten::val yearsArray = emscripten::val::array();
      for (size_t i = 0; i < pair.second.years.size(); ++i) {
        yearsArray.set(i, pair.second.years[i]);
      }
      moveObj.set("years", yearsArray);

      emscripten::val statsMap = emscripten::val::object();
      for (const auto &stat : pair.second.stats) {
        emscripten::val statObj = emscripten::val::object();
        statObj.set("count", stat.second.count);
        statObj.set("points", stat.second.points);
        statsMap.set(std::to_string(stat.first), statObj);
      }
      moveObj.set("stats", statsMap);

      result.set(pair.first, moveObj);
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

class GameData {
public:
  std::vector<ShortMove> moves;
  std::string result;
  int year;
  int id;

  GameData(int id, const std::string &result, int year)
      : id(id), result(result), year(year), moves({}) {}
};
#endif
