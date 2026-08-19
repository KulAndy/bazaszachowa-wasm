#include <unordered_map>
#include <vector>

#include "./Stats.hpp"
#ifndef MOVEDATA_H
#define MOVEDATA_H
struct MoveData {
  int games;
  float points;
  std::vector<int> years;
  std::unordered_map<int, Stats> stats;
};

#endif
